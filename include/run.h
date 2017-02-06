#pragma once

void error_start();

void error_loop();

void error_end();

int start(int *sockfd_out);

int loop(int sockfd);

int end(int sockfd);

void run();
