/*
	B033040013
	Final
	step2 : implement TCP
			add NAT
*/
#include <sys/types.h>
#include <sys/socket.h> 
#include <string.h> 
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include<iostream>
using namespace std;
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <time.h> 
#include <sys/ioctl.h>
#include <net/if.h>
#include <pthread.h>
#include<map>
#define RTT 200
#define DELAY 500
#define THRESHOLD 65535
#define MSS 512
#define BUFFER 10240
#define SERV_PORT 10250
#define CLI_PORT 10260
#define MAX_DATA 1024

struct tcp_seg
{
	uint16_t source_port;
	uint16_t dest_port;
	uint32_t seq_num;
	uint32_t ack_num;
	uint32_t length;
	uint32_t reserved;
	bool URG;
	bool ACK;
	bool PSH;
	bool RST;
	bool SYN;
	bool FIN;
	uint16_t recv_window;
	uint16_t checksum;
	uint16_t urg_ptr;
	unsigned char data[MSS];
	
};

tcp_seg initial(tcp_seg seg_s)
{
	seg_s.source_port = SERV_PORT;
	seg_s.dest_port = CLI_PORT;
	seg_s.seq_num = 0;
	seg_s.ack_num = 0;
 	seg_s.length = 0;
	seg_s.reserved = 0;
	seg_s.URG = 0;
	seg_s.ACK = 0;
	seg_s.PSH = 0;
	seg_s.RST = 0;
	seg_s.SYN = 0;
	seg_s.FIN = 0;
	seg_s.recv_window = htons(THRESHOLD);
	seg_s.checksum = 0;
	seg_s.urg_ptr = 0;
	return seg_s;
}

void recv_error()
{
	printf("receive packet from client ERROR !\n");	
	exit(0);
}

void send_error()
{
	printf("send packet to client ERROR !\n");
	exit(0);
}

int main(int argc, char **argv)
{
	pthread_t thread_num;	//p
	pid_t p1;
	p1=fork();
	if(p1<0)
		cout<<"fork() ERROR !"<<endl;
	else if(p1!=0)
//void server(int argc, char **argv)
  {
	cout<<"SERVER..."<<endl;
	int sockfd;
	char* buffer[1024];
	struct sockaddr_in serv_addr,cli_addr;
	struct ifreq if_info;
	struct tcp_seg seg_s;
	seg_s = initial(seg_s);
    srand(time(NULL));
	//pthread_create(&thread_num, NULL ,/**/  , (void*) /**/);	//p
	//pthread_join( thread_num, &/**/);	//p
	/* create a socket */
	sockfd = socket(AF_INET,SOCK_DGRAM,0); 
	if(sockfd < 0)
	{
		printf("SOCKET FAILED !\n");
		exit(1);	
	}

	/* init serv_addr */
	bzero((char *)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons((unsigned short) atoi(argv[2]));
	
    //if_info.ifr_addr.sa_family = AF_INET;
    //strncpy(if_info.ifr_name, "eth0", IFNAMSIZ-1);
    //ioctl(sockfd, SIOCGIFADDR, &if_info);

	/* bind address and port to socket */
	if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("bind() FAILED !\n");
		exit(1);
	}

	//do_echo(sockfd, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
	socklen_t len = sizeof(cli_addr);
 	//struct sockaddr *pcliaddr = (struct sockaddr *)&cliaddr;
	//pcliaddr = (struct sockaddr *)&cliaddr;
  
	printf("=======Parameter=======\n");
	printf("The RTT delay = %d ms\n",RTT);
	printf("The threshold = %d bytes\n",THRESHOLD);
	printf("The MSS = %d bytes\n",MSS);
	printf("The buffer size = %d bytes\n",BUFFER);
	printf("Server's IP is 127.0.0.1\n");
	cout<<"Server is listening on port "<< argv[2] <<endl;
	printf("========================\n");
	//close(sockfd);

	//while(1)	
	{
		struct sockaddr_in s_addr;
		struct sockaddr_in c_addr;
		if(sockfd < 0)
		{
			perror("SOCKET FAILED !\n");
			exit(0) ;
		}
		memset((char*)&s_addr,0,sizeof(s_addr));		
		s_addr.sin_family = AF_INET;
		s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		s_addr.sin_port = htons((unsigned short) atoi(argv[2]));
		
		memset((char*)&c_addr,0,sizeof(c_addr));		
		c_addr.sin_family = AF_INET;
		c_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		//c_addr.sin_port = htons((unsigned short) atoi(argv[1]));
		c_addr.sin_port = 44444;
//---------------//
//Start Listening//
//---------------//
		printf("Listening......\n");	
		srand(time(NULL));
		tcp_seg recv_seg,send_seg;
		recv_seg = initial(recv_seg);
		send_seg = initial(send_seg);

//Three-way Handshake//

		socklen_t cli_len = sizeof(c_addr);
		//int n;
		//n=recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len);
		if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
		{
			recv_error();
			exit(1);
		}
		printf("=======Start the Three-way Handshake=======\n");
		if(recv_seg.SYN == 1)
		{		
			printf("Receive a packet(SYN) from %s : %d\n", inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
			printf("\tReceive a packet (seq_num = %d , ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num );
		}
		send_seg.seq_num = rand() % 10000;	
		send_seg.ack_num = recv_seg.seq_num + 1;
		send_seg.SYN = 1;	
		send_seg.ACK = 1;
		if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&c_addr,cli_len) < 0)
			send_error();
		printf("Send a packet(SYN/ACK) to %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
		recv_seg = initial(recv_seg);
		if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
			recv_error();
		if(recv_seg.ACK == 1)
		{
			printf("Receive a packet(ACK)  from %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
			printf("\tReceive a packet (seq_num = %d , ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num);
		}
		printf("=======Complete the Three-way Handshake=======\n");

//Data Transfer//

		FILE *fp=fopen("test","rb");
		if( (fp = fopen("test","rb")) <= 0)
			exit(1);
		fseek(fp,0L,SEEK_END);
		int total_size = ftell(fp);
		fclose(fp);
		fp = fopen("test","rb");
		int cwnd = 1, seq = 1, size = 1, rwnd = total_size;
		int cwnd_t;
		int out = 0;
		int check = 0;
		printf("Start to send the file, the file is %d bytes\n********Slow Start********\n", total_size);
		send_seg.recv_window = total_size;
		while(rwnd >= MSS)
		{
			if(cwnd <= MSS)		//slow start
			{
				size=fread(send_seg.data,sizeof(unsigned char),cwnd,fp);
				send_seg = initial(send_seg);
				send_seg.seq_num = seq;
				send_seg.ack_num = recv_seg.seq_num + 1;
				send_seg.recv_window = rwnd;
				if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&c_addr,cli_len) < 0)
					send_error();
				if(check != cwnd)
				{
					printf("cwnd = %d, rwnd = %d, threshold = %d\n\tSend a packet at: %d byte\n",cwnd,rwnd,THRESHOLD,seq);
					check = cwnd;
				}
				else
					printf("\tSend a packet at: %d byte\n",seq);
				recv_seg = initial(recv_seg);
				if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
					recv_error();
				printf("\tReceive a packet (seq_num = %d , ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num);
				rwnd = BUFFER - size;
				seq = seq + size;
				if(cwnd * 2 < rwnd)
					cwnd = cwnd*2;
			}
			else		//cogestion avoidence
			{
				int cnt = 0;
				check = 0;
				cwnd_t = cwnd;		
				send_seg = initial(send_seg);
				send_seg.seq_num = seq;
				send_seg.ack_num = recv_seg.seq_num + 1;
				while(cwnd_t > 0)
				{
					cwnd_t = cwnd_t - MSS;
					size = fread(send_seg.data , sizeof(unsigned char) , MSS , fp);
					if(feof(fp) || seq >= 10000)
					{
						out = 1;
						send_seg.FIN = 1;
					}
					send_seg.recv_window = rwnd;
					if( sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&c_addr,cli_len) < 0)
						send_error();
					if(check != cwnd)
					{
						printf("cwnd = %d, rwnd = %d, threshold = %d\n\tSend a packet at: %d byte\n",cwnd,rwnd,THRESHOLD,seq);
						check = cwnd;
					}
					else
						printf("\tSend a packet at: %d byte\n",seq);
					seq = seq + MSS;		
					cnt++;
					send_seg.seq_num = send_seg.seq_num + MSS;
					send_seg.ack_num++;
					if(out == 1)			
						break;
				}
				rwnd = 2 * rwnd - BUFFER;		
				while(cnt > 0)
				{
				
					if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
						recv_error();
					printf("\tReceive a packet (seq_num = %d , ack_num = %d)\n", recv_seg.seq_num,recv_seg.ack_num);		
					if(recv_seg.FIN == 1)
					{
						printf("================Data transfer done=================\n");
						break;	
					}
					cnt--;
				}
				if((cwnd*2 < rwnd) && (cwnd*2 < THRESHOLD))
					cwnd = cwnd * 2;
				else if(cwnd + MSS < rwnd)
					cwnd = cwnd + MSS;
				if(out == 1)
					break;
			}
		}
		fclose(fp);
		send_seg.FIN = 0;
		send_seg.ACK = 0;
		
//Four-way Handshake

		send_seg = initial(send_seg);		
		send_seg.seq_num = recv_seg.ack_num - 1;
		send_seg.ack_num = recv_seg.seq_num + 1;
		send_seg.FIN = 1;
		if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&c_addr,cli_len) < 0)
			send_error();
		printf("============Start the four-way handshake===========\n");
		printf("Send a packet(FIN) to %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));	
		recv_seg = initial(recv_seg);
		if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
			recv_error();
		if(recv_seg.ACK == 1)
		{
			printf("Receive a packet(ACK) from %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
			printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num);
		}

		recv_seg = initial(recv_seg);
		if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&c_addr,&cli_len) < 0)
			recv_error();
		if(recv_seg.FIN == 1)
		{
			printf("Receive a packet(FIN) from %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
			printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num);
		}
		send_seg = initial(send_seg);
		send_seg.seq_num = recv_seg.ack_num;
		send_seg.ack_num = recv_seg.seq_num + 1;
		send_seg.ACK = 1;
		if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&c_addr,cli_len) < 0)
			send_error();
		printf("Send a packet(ACK) to %s : %d\n",inet_ntoa(c_addr.sin_addr),htons(c_addr.sin_port));
		printf("==========Complete the four-way handshake=========\n"); 
		close(sockfd);
	}
  }
  else
//void client(int argc, char **argv)
  {
	
	cout<<"CLIENT..."<<endl;
	cout<<"?"<<endl;
	int sockfd;
	struct sockaddr_in serv_addr,cli_addr;
	/* check args */
	if(argc != 3)
	{
		printf("usage: ./c <IP> <PORT>\n");
		exit(1);
	}
	/* create a socket */
	sockfd = socket(AF_INET,SOCK_DGRAM,0); 
	if(sockfd < 0)
	{
		printf("SOCKET FAILED !\n");
		exit(1);	
	}
	memset((char*)&cli_addr,0,sizeof(cli_addr));
	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	cli_addr.sin_port = htons((unsigned short) atoi(argv[2]));
	//cli_addr.sin_port = 44444;
	memset((char*)&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	unsigned int c_port;
	cin>>c_port;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(c_port);
	//192.168.40.128
	//inet_pton(AF_INET,argv[1],&serv_addr.sin_addr);
	/* bind address and port to socket */
/*	
	if(bind(sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) == -1)
	{
		perror("bind() FAILED !\n");
		exit(1);
	}
*/
	//NAT//

	map<pair<string,unsigned int>, pair<string,unsigned int> > myMap;
    map<pair<string,unsigned int>, pair<string,unsigned int> >::iterator i;
    myMap.insert(std::make_pair(std::make_pair("192.168.40.129",10260),std::make_pair("192.168.40.128",10260)));
    myMap.insert(std::make_pair(std::make_pair("192.168.40.130",10265),std::make_pair("192.168.40.128",10265)));
    struct in_addr ip_addr;
    printf("-------------NAT translation table-------------\n");
    printf("  WAN side addr\t|\tLAN side addr\n");
    for(i = myMap.begin();i != myMap.end();i++)
        printf("%s : %d \t \t %s : %d\n",i->second.first.c_str(),i->second.second,i->first.first.c_str(),i->first.second);
    string nat_ip(inet_ntoa(cli_addr.sin_addr));
    printf("Client's IP is 192.168.40.128\n");
    cli_addr.sin_addr.s_addr = inet_addr(myMap[make_pair(nat_ip,ntohs(cli_addr.sin_port))].first.c_str());
	

	srand(time(NULL));
	tcp_seg recv_seg,send_seg;
	recv_seg = initial(recv_seg);
	send_seg = initial(send_seg);
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
	{
		perror("connect error");
		exit(1);
	}

//Three-way Handshake//

	socklen_t serv_len = sizeof(serv_addr);
	send_seg.seq_num = rand() % 10000;	
	send_seg.SYN = 1;
	if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr*)&serv_addr,serv_len) < 0)
		send_error();
	printf("========Start the Three-way Handshake========\n");
	printf("Send a packet(SYN) to %s : %d\n",inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
	if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&serv_addr,&serv_len) < 0)
		recv_error();
	if(recv_seg.ACK == 1 && recv_seg.SYN == 1)
	{
		printf("Receive a packet(SYN/ACK) from %s : %d\n",inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
		printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num);
	}
	send_seg = initial(send_seg);
	send_seg.seq_num = recv_seg.ack_num;
	send_seg.ack_num = recv_seg.seq_num+1;		
	send_seg.ACK = 1;
	if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr *)&serv_addr,serv_len ) < 0)
		send_error();
	printf("Send a packet(ACK) to %s : %d\n",inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
	printf("=======Complete the Three-way Handshake=======\n");
	
//Data Transfer//
	
	printf("Receive a file from %s : %d\n", inet_ntoa(serv_addr.sin_addr) , htons(serv_addr.sin_port) );
	while(1)
	{
		if(recvfrom(sockfd , (char*)&recv_seg , sizeof(recv_seg) ,0 , (struct sockaddr *)&serv_addr , &serv_len) < 0)
			recv_error();
		printf("\tReceive a packet (seq_num = %d , ack_num = %d)\n", recv_seg.seq_num, recv_seg.ack_num);
			
		send_seg.seq_num = recv_seg.ack_num;
		send_seg.ack_num = recv_seg.seq_num * 2;		
		if(recv_seg.seq_num > MSS )
			send_seg.ack_num = recv_seg.seq_num + 512;		
		if(recv_seg.FIN == 1)
			send_seg.FIN = 1;
		if(sendto(sockfd , (char*)&send_seg , sizeof(send_seg) , 0 , (struct sockaddr *)&serv_addr , serv_len ) < 0 )
			send_error();			
		if(recv_seg.FIN == 1)
			break;
	}
	printf("==============Data transfer done==============\n");
	send_seg.ACK = 0;

//Four-way Handshake

	recv_seg = initial(recv_seg);
	if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&serv_addr,&serv_len) < 0)
		recv_error();
	printf("============Start the four-way handshake===========\n");
	if(recv_seg.FIN == 1)
	{
		printf("Receive a packet(FIN) from %s : %d\n", inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
		printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n",recv_seg.seq_num,recv_seg.ack_num );
	}
	
	send_seg = initial(send_seg);
	send_seg.seq_num = recv_seg.ack_num;
	send_seg.ack_num = recv_seg.seq_num + 1;
	send_seg.ACK = 1;
	if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr *)&serv_addr,serv_len) < 0)
		send_error();
	printf("Send a packet(ACK) to %s : %d\n", inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
		
	send_seg.FIN=1;
	if(sendto(sockfd,(char*)&send_seg,sizeof(send_seg),0,(struct sockaddr *)&serv_addr,serv_len) < 0)
		send_error();
	printf("Send a packet(FIN) to %s : %d\n", inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));

	recv_seg = initial(recv_seg);
	if(recvfrom(sockfd,(char*)&recv_seg,sizeof(recv_seg),0,(struct sockaddr *)&serv_addr,&serv_len) < 0)
		recv_error();
	if(recv_seg.ACK == 1)
	{
		printf("Receive a packet(ACK) from %s : %d\n", inet_ntoa(serv_addr.sin_addr),htons(serv_addr.sin_port));
		printf("\tReceive a packet (seq_num = %d, ack_num = %d)\n", recv_seg.seq_num,recv_seg.ack_num);
	}
	printf("==========Complete the four-way handshake=========\n");
	close(sockfd);
  }
	return 0;
}
/*
int main(int argc, char **argv)
{
	
	pid_t p;
	p=fork();
	if(p<0)
		cout<<"fork() ERROR !"<<endl;
	else if(p==0)
		client(argc,**argv);
	else
		server(argc,**argv);
	
	//pthread_t thread_num;	//p
	
	return 0;
}*/
