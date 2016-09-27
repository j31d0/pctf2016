#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define MAXRECV (1024*1024*10)
#define NUM_CONNECTION 200

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
unsigned short make_checksum(unsigned char* buf, unsigned int len);
unsigned short add_checksum(unsigned char* buf, unsigned int len);
void CEASER_ENCRYPT(unsigned char* dest, unsigned char* src, unsigned int len, unsigned char shift, unsigned char op);

int con = -1;
struct sockaddr_in mysoc;
unsigned int port;

// to make graceful exit
void sig_int_stop_handler(int sig)
{
    if(con != -1)
	close(con);
    exit(0);
}

// reaping child
void sigchld_handler(int sig) 
{
    int status;
    pid_t pid;
    pid = waitpid(-1,&status,WNOHANG | WUNTRACED);
    if(!pid) 
	return;
    //if child was exit or was killed by signal (error or something)
    if(WIFSIGNALED(status) || WIFEXITED(status))
    {
        kill(pid, SIGKILL);
#ifdef DEBUG
	printf("kill %d\n", pid);
#endif
    }
    return;
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

    signal(SIGCHLD, sigchld_handler);
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
    mysoc.sin_addr.s_addr = INADDR_ANY;
    
    int binding = -1;

    while (binding == -1)
    {
        binding = bind(con,(struct sockaddr*)&mysoc, sizeof(mysoc));
	if(binding == -1)
	{
	    sleep(2);
	}
    }

    listen(con,NUM_CONNECTION);

    int newfd = 0;
    while(1)
    {
	struct sockaddr_in cli_addr;
	int cli_len = sizeof(cli_addr);
	newfd = accept(con, (struct sockaddr*)&cli_addr, &cli_len);
	if(!fork())
	    break;
	close(newfd);
    }

    do_serve(newfd);
    close(newfd);
    close(con);

    return 0;

}

void do_serve(int sockfd)
{
    while(1){
#ifdef DEBUG
	printf("start serve %d\n", sockfd);
#endif
    struct ceaser_header tmp_header;
    unsigned int  left = sizeof(struct ceaser_header);
    unsigned int recved_len = 0;
    while(left)
    {
	recved_len = recv(sockfd, (unsigned char*)(&tmp_header) + sizeof(struct ceaser_header) - left, left,0);

	#ifdef DEBUG
	printf("recved %d\n", recved_len);
#endif
        if(recved_len == -1)
	{
	    bad_error("recv fail");
	}
	else if (recved_len == 0)
	{
	    return;
	}
	else
	{
	    left -= recved_len;
	}
    }

#ifdef DEBUG
    printf("recved all header\n");
    printf("op : %d\nshift : %d\n,checksum : %hx\n",tmp_header.op, tmp_header.shift, tmp_header.checksum);
#endif
    if((tmp_header.op != 1) && (tmp_header.op != 0)) return;

   unsigned int packet_len = ntohl(tmp_header.length);

    // check length field
    if (packet_len > MAXRECV) return;
    if (packet_len < sizeof(struct ceaser_header)) return;

    unsigned char* recv_buf = (unsigned char*) malloc(packet_len);
    if(!recv_buf)
    {
	bad_error("allocation failed");
	return;
    }
    
    memcpy(recv_buf,&tmp_header,sizeof(struct ceaser_header));

#ifdef DEBUG
    printf("RECV header\n");
    int i = 0;
    for(i =  0; i < sizeof(struct ceaser_header); i++)
    {
	printf("%02x", recv_buf[i]);
    }
    printf("\n");
#endif



    left = packet_len - sizeof(struct ceaser_header);
    recved_len = 0;
    while(left)
    {
	recved_len = recv(sockfd, recv_buf + packet_len - left, left, 0);
	if (recved_len == -1)
	{
	    bad_error("recv fail");
	}
	else if (recved_len == 0)
	{
	    return;
	}
	else
	{
	    left -= recved_len;
	}
    }
    


#ifdef DEBUG
    printf("RECV body\n");
#endif


    if(add_checksum(recv_buf,packet_len) != (unsigned short)0xffff)
    {
	bad_error("checksum not correct");
	free(recv_buf);
	return;
    }

    struct ceaser_header* send_header = (struct ceaser_header*)recv_buf;
    send_header->checksum = 0;

    CEASER_ENCRYPT((recv_buf + sizeof(struct ceaser_header)), (recv_buf + sizeof(struct ceaser_header)), packet_len - sizeof(struct ceaser_header), send_header->shift, send_header->op);

    unsigned short check = make_checksum(recv_buf, packet_len);
    send_header->checksum = check;


    left = packet_len;
#ifdef DEBUG
    printf("send start : %d\n", left);
#endif
    unsigned int sended_len = 0;
    while(left)
    {
        sended_len = send(sockfd, recv_buf + packet_len - left, left, 0);
	if(sended_len == -1)
	{
	    free(recv_buf);
	    bad_error("send fail");
	    return;
	}
	else
	{
	    left -= sended_len;
#ifdef DEBUG
	    printf("left : %d\n", left);
#endif
	}
    }

    free(recv_buf);
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
	printf("Usage: ./server -p port\n");
    }
}


