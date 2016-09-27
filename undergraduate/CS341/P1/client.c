#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define MAXLEN (1024*16)

// buflist : to handle large input
struct buflist
{
    struct buflist* next;
    unsigned int len;
    unsigned char buf[MAXLEN + 1];
};


// ceaser cipher protocol header
struct ceaser_header
{
    unsigned char op;
    unsigned char shift;
    unsigned short checksum;
    unsigned int length;
};

void show_usage(int argc, char** argv);
void bad_error(char* msg);
void do_serve(int sockfd);
void send_and_get(int sockfd, struct buflist* head);
void send_message(int sockfd, struct buflist* buf, unsigned int operation, unsigned int shift);
void get_message(int sockfd);
void free_all(struct buflist* head);
struct buflist* make_buflist();
unsigned short make_checksum(unsigned char* buf, unsigned int len);
unsigned short add_checksum(unsigned char* buf, unsigned int len);
unsigned int secure_len(unsigned char* buf, unsigned int len);

int con;
struct sockaddr_in mysoc;
unsigned int port;
unsigned int operation;
unsigned int shift;
unsigned char* address;
struct buflist* head;

int main(int argc, char** argv)
{
    char opt;
    int check = 0;
    // get option with getopt
    while((opt = getopt(argc,argv,"h:p:o:s:")) != -1)
    {
	switch(opt)
	{
	    case 'h':
	        address = (unsigned char*)malloc(strlen(optarg) + 1);
	        if(address)
	       	    strcpy(address, optarg);
	        else
		    bad_error("allocation fail");
	        check |= 0x1;
	        break;
            case 'p':
	        port = atoi(optarg);
	        check |= 0x2;
	        break;
	    case 'o':
	        operation = atoi(optarg);
	        check |= 0x4;
	        break;
	    case 's':
	        shift = atoi(optarg);
	        check |= 0x8;
	        break;
	}
    }
    
    // check whether all arguments are given
    if(check != 0xf)
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
    mysoc.sin_addr.s_addr = inet_addr(address);

    // if inet_addr failed, exit
    if(mysoc.sin_addr.s_addr == INADDR_NONE)
	bad_error("unknown host");
    
    int conRes = -1;

    while (conRes == -1)
    {
        conRes = connect(con,(struct sockaddr*)&mysoc, sizeof(mysoc));
	if(conRes == -1)
	{
	    sleep(2);
	}
    }

    // main service loop
    do_serve(con);

    close(con);

    return 0;

}

void do_serve(int sockfd)
{
    // make initial buffer to buflist head
    head = make_buflist();
    struct buflist* cur = head;
    while(!feof(stdin))
    {
	memset(cur->buf, '\x01', MAXLEN + 1);
	char* result = fgets(cur->buf, MAXLEN + 1, stdin);
	if(result == NULL && feof(stdin))
	    break;
	else if(result == NULL)
	    bad_error("fgets failed.");

	// to get real length which is given by fgets (to handle '\x00' case)
	cur->len = secure_len(result, MAXLEN + 1);
	if(cur->len == 0) continue; 

        // if fgets() detect '\n', send to server and get message.
	if (cur->buf[cur->len - 1] == '\n')
	{
	    send_and_get(sockfd,head);
	    free_all(head);
	    head = make_buflist();
	    cur = head;
	} // else, expand buffer and get more input
	else
	{
	    cur->next = make_buflist();
	    cur = cur->next;
	}

    }

    // if eof is reached, flush remaining buffer
    send_and_get(sockfd,head);
    return;
}

// to get largest i with buf[i] == '\x01' (assume fgets are called with buf)
unsigned int secure_len(unsigned char* buf,unsigned int len)
{
    unsigned int i = len - 1;
    while(i > 0 && buf[i--] == '\x01');
    return i + 1;

}

// free all buffer list
void free_all(struct buflist* head)
{
    struct buflist* cur = head;
    struct buflist* tmp = NULL;
    while(cur)
    {
	tmp = cur->next;
	free(cur);
	cur = tmp;
    }
    return;

}

// send each messages in buffer list, and print recved message.
void send_and_get(int sockfd, struct buflist* head)
{
    struct buflist* cur = head;
    while(cur)
    {
	if(cur->len == 0)
	{
	    cur = cur->next;
	    continue;
	}

	send_message(sockfd,cur,operation,shift);
#ifdef DEBUG
	printf("SEND OK! GET..\n");
#endif
	get_message(sockfd);
#ifdef DEBUG
	printf("GET OK! print..\n");
#endif
	cur = cur->next;
    }

    return;

}

void send_message(int sockfd, struct buflist* buf, unsigned int operation, unsigned int shift)
{
    unsigned int sending_len = buf->len + sizeof(struct ceaser_header);
    unsigned char* sending_buffer = (unsigned char*)malloc(sending_len);
    if(!sending_buffer)
    {
	bad_error("allocation failed");
    }
    struct ceaser_header* header = (struct ceaser_header*)sending_buffer;
    header->op = (unsigned char)operation;
    header->shift = (unsigned char)(shift % 26);
    header->checksum = 0;
    header->length = htonl(sending_len);
    memcpy(sending_buffer + sizeof(struct ceaser_header), buf->buf, buf->len);


    unsigned short checksum = make_checksum(sending_buffer,sending_len);
    header->checksum = checksum;

    //sending procedure
    unsigned int left = sending_len;
    while(left)
    {
	int sended_len = send(sockfd, sending_buffer + sending_len - left, left,0);
	if(sended_len == -1)
	{
	    if (errno == EBADF || errno == ECONNRESET || errno == EFAULT || errno == EINVAL)
		bad_error("send fail..");
	    continue;
	}
	else
	    left -= sended_len;
    }

    return;

}

void get_message(int sockfd)
{
    unsigned int recving_len = sizeof(struct ceaser_header);
    unsigned int left = recving_len;
    struct ceaser_header header;
    // header recv
    while(left)
    {
	int recved_len = recv(sockfd, ((unsigned char*)&header) + recving_len - left, left, 0);
	if(recved_len == -1)
	{
	    if(errno == EBADF || errno == ECONNRESET || errno == EFAULT || errno == EINVAL)
		bad_error("recv fail..");
	    continue;
	}
	else
	{
	    left -= recved_len;
	}
    }

    unsigned int len = ntohl(header.length);
    if (len < sizeof(struct ceaser_header))
	bad_error("protocol-length is small");
    
    unsigned char* recv_buf = (unsigned char*)malloc(len);
    if(!recv_buf)
    {
	bad_error("allocation fail");
    }

    memcpy(recv_buf,&header, sizeof(struct ceaser_header));
    recving_len = len - sizeof(struct ceaser_header);
    left = recving_len;

    while(left)
    {
	int recved_len = recv(sockfd, (recv_buf + sizeof(struct ceaser_header)) + recving_len - left, left, 0);
	if(recved_len == -1)
	{
	    if(errno == EBADF || errno == ECONNRESET || errno == EFAULT || errno ==EINVAL)
		bad_error("recv fail..");
	    continue;
	}
	else
	{
	    left -= recved_len;
	}
    }

    int i =0;
#ifdef DEBUG
    for(; i < len; i++)
    {
	printf("%02x ", recv_buf[i]);
    }
    printf("\n");
#endif
    unsigned short check = add_checksum(recv_buf, len);

#ifdef DEBUG
    printf("%x\n", check);
#endif
    if(check != (unsigned short)0xffff)
    {
	bad_error("not correct checksum");
    }

    fwrite(recv_buf + sizeof(struct ceaser_header), len - sizeof(struct ceaser_header), 1, stdout);
    return;
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
	printf("Usage: %s -h hostname -p port -o operation -s shift\n", argv[0]);
    }
    else
    {
	printf("Usage: ./client -h hostname -p port -o operation -s shift\n");
    }
}


struct buflist* make_buflist()
{
    struct buflist* a= (struct buflist*)malloc(sizeof(struct buflist));
    if(a == NULL)
    {
	bad_error("allocation fail");
    }
    a->len = 0;
    a->next =NULL;
    return a;
}
