#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define MAXRECV (1024*1024*10)
#define NUM_CONNECTION 200
#define LIMIT_ONCE (1024 * 512)

struct ceaser_header
{
    unsigned char op;
    unsigned char shift;
    unsigned short checksum;
    unsigned int length;
};


// client_list for handling multiple client concurrently
struct client_list
{
    unsigned char active; // 0 : have to remove, 1 : alive
    unsigned char state; // 0 : read header, 1 : read body, 2: write, 3: init
    int fd;
    unsigned int left_len; // left for read/write
    struct ceaser_header header;
    unsigned int buf_len;
    unsigned char* buf;
    struct client_list* next;
};



void show_usage(int argc, char** argv);
void bad_error(char* msg);
void do_serve(struct client_list* iter);
void add_to_list(int fd);
void delete_to_list(struct client_list* prev, struct client_list* cur);
unsigned short make_checksum(unsigned char* buf, unsigned int len);
unsigned short add_checksum(unsigned char* buf, unsigned int len);
void CEASER_ENCRYPT(unsigned char* dest, unsigned char* src, unsigned int len, unsigned char shift, unsigned char op);

int con = -1;
struct sockaddr_in mysoc;
struct client_list* client_head = NULL;
unsigned int port;
int max_fd = 0;
fd_set read_list,write_list,read_res,write_res;


void sig_int_stop_handler(int sig)
{
    if(con != -1)
	close(con);
    exit(0);
}


void CEASER_ENCRYPT(unsigned char* dest, unsigned char* src, unsigned int len, unsigned char shift, unsigned char op)
{
    unsigned int i =0;
    for(; i < len; i++)
    {
	if(src[i] >= 'a' && src[i] <= 'z')
	{
	    if(op == 0)
	    {
		dest[i] = ((src[i] - 'a' + shift) % 26) + 'a';
	    }
	    else if(op == 1)
	    {
		dest[i] = ((src[i] - 'a' - (shift % 26) + 26 ) % 26) + 'a';
	    }
	}

	if(src[i] >= 'A' && src[i] <= 'Z')
	{
	    if(op == 0)
	    {
		dest[i] = ((src[i] - 'A' + shift) % 26) + 'a';
	    }
	    else if(op == 1)
	    {
		dest[i] = ((src[i] - 'A' - (shift % 26) + 26 ) % 26) + 'a';
	    }
	}

    }
    return;
}

int main(int argc, char** argv)
{

    signal(SIGINT, sig_int_stop_handler);
    signal(SIGSTOP, sig_int_stop_handler);

    char opt;
    int check = 0;
    while((opt = getopt(argc,argv,"p:")) != -1)
    {
	switch(opt)
	{
            case 'p':
	        port = atoi(optarg);
	        check |= 0x2;
	        break;
	}
    }
    if(check != 0x2)
    {
	show_usage(argc,argv);
	exit(0);
    }
    
    con = socket(AF_INET, SOCK_STREAM, 0);
    if(con == -1)
    {
	bad_error("socket()");
    }

    memset(&mysoc, 0, sizeof(mysoc));
    mysoc.sin_family = AF_INET;
    mysoc.sin_port = htons(port);
    mysoc.sin_addr.s_addr = htonl(INADDR_ANY);
    
    int binding = -1;

    while (binding == -1)
    {
        binding = bind(con,(struct sockaddr*)&mysoc, sizeof(mysoc));
	if(binding == -1)
	{
	    perror("bind");
	    sleep(2);
	}
    }
    if(listen(con,NUM_CONNECTION)) perror("listen");

    // init fd_set
    int new_fd = 0;
    FD_ZERO(&read_list);
    FD_ZERO(&write_list);
    FD_ZERO(&read_res);
    FD_ZERO(&write_res);

    FD_SET(con, &read_list);
    max_fd = con;

    while(1)
    {
	read_res = read_list;
	write_res = write_list;
	if(select(max_fd + 1, &read_res, &write_res, (fd_set*)0, NULL) == -1)
	{
	    perror("select");
	    exit(0);
	}

	if (FD_ISSET(con, &read_res))
	{
	    struct sockaddr cli_addr;
	    int cli_len = sizeof(cli_addr);
	    new_fd = accept(con, (struct sockaddr * )&cli_addr, &cli_len);
	    if(new_fd == -1)
	    {
		perror("accept");
		exit(0);
	    }
	    
	    //change accepted fd's property to non-blocking socket
	    int flg = fcntl(new_fd, F_GETFL);
	    if(flg < 0) perror("fcntl1");
	    if(fcntl(new_fd, F_SETFL, flg | O_NONBLOCK) < 0) perror("fcntl2");
	    add_to_list(new_fd);
	    FD_SET(new_fd, &read_list);
#ifdef DEBUG
	    printf("new! %d\n", new_fd);
#endif

	    if(new_fd > max_fd)
	    {
		max_fd = new_fd;
	    }
	}

	struct client_list *cur = client_head;
#ifdef DEBUG
	printf("head : %p\n", client_head);
#endif
	// main service loop
	while(cur)
	{
#ifdef DEBUG
	    printf("Start serve\n");
#endif
	    do_serve(cur);
	    cur = cur->next;
#ifdef DEBUG
	    printf("End serve\n");
#endif
	}

        // delete connection-closed, and errorness client
	struct client_list *prev = NULL;
	cur = client_head;
	struct client_list* tmp;
	while(cur)
	{
	    tmp = cur->next;
	    if(cur->active == 0)
	    {
		delete_to_list(prev,cur);
	    }
	    else
	    {
		prev = cur;
	    }
	    cur = tmp;
	}

    }

    // can't reach
    close(con);
    return 0;

}

void add_to_list(int fd)
{
    struct client_list* retVal = (struct client_list*)malloc(sizeof(struct client_list));
    if(!retVal)
    {
	perror("add_to_list-malloc");
	return;
    }

    memset(retVal,'\x00',sizeof(struct client_list));
    retVal->active = 1;
    retVal->state = 3;
    retVal->buf_len = sizeof(struct ceaser_header);
    retVal->left_len = sizeof(struct ceaser_header);
    retVal->fd = fd;

    if(!client_head)
    {
	client_head = retVal;
	return;
    }

    struct client_list* cur = client_head;
    while(cur->next) cur = cur->next;
    cur->next = retVal;
    return;

}

void delete_to_list(struct client_list* prev, struct client_list *cur)
{
#ifdef DEBUG
    printf("DELETE\n");
#endif
    if(!prev)
    {
	client_head = cur->next;
	if(cur->buf) free(cur->buf);
	FD_CLR(cur->fd, &write_list);
	FD_CLR(cur->fd, &read_list);
	close(cur->fd);
	free(cur);
    }else
    {
	prev->next = cur->next;
	if(cur->buf) free(cur->buf);
	FD_CLR(cur->fd, &write_list);
	FD_CLR(cur->fd, &read_list);
	close(cur->fd);
	free(cur);
    }
}

void do_serve(struct client_list *iter)
{
    #ifdef DEBUG
    printf("SERVE %p\n", iter);
    if(iter)
    {
	printf("active: %d\nstate: %d\nleft_len: %d\nbuf_len: %d\nbuf: %p\n", iter->active, iter->state, iter->left_len, iter->buf_len, iter->buf);
    }
    #endif
    if(!iter || !iter->active) return;

    if(iter->state == 3) // first state
    {
	iter->state = 0; // ready to recv
	/* I didn't set client's initial state to 0 because
	 * at first time (fd is inserted to fd_set), it cannot be 
	 * guaranteed that fd is read/write-able.
	 */
	return;
    }

    int sockfd = iter->fd;

    // 0 : recving header
    if((iter->state == 0) && FD_ISSET(sockfd,&read_res))
    {
	unsigned int recved_len = 0;

	// try to recv header
	recved_len = recv(sockfd, ((char*)&iter->header) + iter->buf_len - iter->left_len  , iter->left_len, 0);
	if(recved_len == -1 && errno == EAGAIN)
	{
	    return; // pass
	}
	else if((recved_len == 0) || (recved_len == -1))
	{
	    //exit and error : remove fd
	    iter->active = 0;
	    return;
	}
	else
	{
	    //get data
	    iter->left_len -= recved_len;
	    if (!iter->left_len) // if recved all header
	    {
		if ((iter->header.op != 1) && (iter->header.op != 0))
		{
		    //close connection
		    iter->active = 0;
		    return;
		}
		
		unsigned int packet_len = ntohl(iter->header.length);
		if(packet_len > MAXRECV)
		{
		    iter->active = 0;
		    return;
		}
		if(packet_len < sizeof(struct ceaser_header))
		{
		    iter->active = 0;
		    return;
		}
		iter->buf = (unsigned char*)malloc(packet_len);
		if(!iter->buf)
		{
		    perror("malloc");
		    iter->active = 0;
		    return;
		}
		memcpy(iter->buf,(char*)&iter->header,sizeof(struct ceaser_header));
		iter->buf_len = packet_len;

		iter->state = 1;
		// have to recv (header->length - sizeof(header))
		iter->left_len = packet_len - sizeof(struct ceaser_header);
#ifdef DEBUG
        printf("RECV header\n");
        int i = 0;
        for(i =  0; i < sizeof(struct ceaser_header); i++)
        {
            printf("%02x", iter->buf[i]);
        }
        printf("\n");
#endif


	    }
	}
	return;

    }

    // 1 : recving body
    if((iter->state == 1) && FD_ISSET(sockfd, &read_res))
    {
        unsigned int recved_len = 0;
	unsigned int sum_recved = 0;

	// try to recv body
        while(sum_recved < LIMIT_ONCE && iter->left_len)
        {
	    recved_len = recv(sockfd, iter->buf + iter->buf_len - iter->left_len, iter->left_len, 0);
	    if (recved_len == -1 && errno == EAGAIN) // buffer empty
	    {
		return; //pass
	    }
	    else if (recved_len == 0 || recved_len == -1)
	    {
		iter->active = 0; // remove
	        return;
	    }
	    else 
	    {
	        iter->left_len -= recved_len;
		sum_recved += recved_len;
	    }
        }

	// if client send soooo much data, i just recv at most LIMIT_ONCE
	if(iter->left_len) return;



#ifdef DEBUG
        printf("RECV body\n");
#endif


        if(add_checksum(iter->buf,iter->buf_len) != (unsigned short)0xffff)
        {
#ifdef DEBUG
	    printf("fd: %d checksum error", iter->fd);
#endif
	    iter->active = 0; // die
	    return;
        }

        struct ceaser_header* send_header = (struct ceaser_header*)iter->buf;
        send_header->checksum = 0;

        CEASER_ENCRYPT((iter->buf + sizeof(struct ceaser_header)), (iter->buf + sizeof(struct ceaser_header)), iter->buf_len - sizeof(struct ceaser_header), send_header->shift, send_header->op);

        unsigned short check = make_checksum(iter->buf, iter->buf_len);
        send_header->checksum = check;

	//recv end, so change state to send
	iter->state = 2;
	iter->left_len = iter->buf_len; // have to send all buf
	FD_CLR(sockfd, &read_list);
	FD_SET(sockfd, &write_list);
	return;
    }

    if((iter->state == 2) && FD_ISSET(sockfd, &write_res))
    {
#ifdef DEBUG
        printf("send start : %d\n", iter->left_len);
#endif
        unsigned int sended_len = 0;
	unsigned int sum_send = 0;
        while(sum_send < LIMIT_ONCE && iter->left_len)
        {
            sended_len = send(sockfd, iter->buf + iter->buf_len - iter->left_len, iter->left_len, 0);
	    if(sended_len == -1 && errno == EAGAIN)
	    {
	        return;
	    }
	    else if (sended_len == 0 || sended_len == -1)
	    {
	        iter->active = 0;
		return;
	    }else
	    {
		iter->left_len -= sended_len;
		sum_send += sended_len;
#ifdef DEBUG
	        printf("left : %d\n", iter->left_len);
#endif
	    }
        }

        // if I send LIMIT_ONCE at once, try to send next time
	if(iter->left_len) return; 


	//send end, so change state to recv_header
        iter->state = 0;
	free(iter->buf);
	iter->buf = NULL;
	iter->buf_len = sizeof(struct ceaser_header);
	iter->left_len = sizeof(struct ceaser_header); // have to recv header
        FD_CLR(sockfd, &write_list);
        FD_SET(sockfd, &read_list);
	return;
    }

}


// this checksum algorithm are taken from http://locklessinc.com/articles/tcp_checksum/, but modified
unsigned short add_checksum(unsigned char* buf, unsigned int len)
{
    unsigned int neg = 0;
    unsigned int i = 0;
    for(i = 0; i < len - 1; i+= 2)
    {
	neg += *(unsigned short*)(buf+i);
	neg = (neg & 0xffff) + (neg >> 16);
    }
    if(len & 1)
    {
	neg += *(unsigned char*)(buf + i);
	neg = (neg & 0xffff) + (neg >> 16);
    }
    return neg;


}

unsigned short make_checksum(unsigned char* buf, unsigned int len)
{

    return ~add_checksum(buf,len);
   
}

void bad_error(char* msg)
{
    printf("Error: %s\n" , msg);
    exit(-1);
}

void show_usage(int argc, char** argv)
{
    if(((unsigned int)argc) > 0)
    {
	printf("Usage: %s -p port\n", argv[0]);
    }
    else
    {
	printf("Usage: ./server_select -p port\n");
    }
}


