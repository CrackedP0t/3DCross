#pragma once

#define SERVER_IP "192.168.0.21"
#define SERVER_PORT 14500

int begin_connect(int *sockfd_out);

int end_connect(int sockfd);
