#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <ctype.h>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <string>
#define BUF_SIZE (4096)
#define BUF_SIZE_1 (1000000)
#define SEM_SIZE (20) // 群聊上限人数
 
// 信号量--判断群聊人数
sem_t sem;
// 服务端文件描述符
int svr_fd;
// 存储群友，多一个是为了当群人满时，空一个出来接发信息
int cli_fd[SEM_SIZE + 1] = {};
 
struct client
{
	/* data */
	char buf[BUF_SIZE];	 // message
	char name[BUF_SIZE]; // name
	int client_fd;		 // fd
};
 
struct client clients[SEM_SIZE];


// void handle_video(int fd) {
//     FILE *file = fopen("received_video.mp4", "wb");
//     if (!file) {
//         perror("Failed to open file for writing");
//         return;
//     }

//     char buffer[BUF_SIZE];  // Ensure there's space for null termination
//     int bytes_read = read(fd, buffer, BUF_SIZE); // Read video length
//     if (bytes_read <= 0) {
//         perror("Failed to read from socket");
//         fclose(file);
//         return;
//     }

//     // buffer[bytes_read] = '\0';  // Null-terminate the buffer for string operations
//     printf("Bytes_read: %d\n", bytes_read);
//     printf("What's in buffer: %s\n", buffer);
//     // Locate the delimiter to find the end of the size string

//     // int length_bytes_read = read(fd, buffer, BUF_SIZE);
//     // printf("Length_bytes_read: %d\n", bytes_read);
//     // printf("What's in buffer: %s\n", buffer);
//     char* delimiterPosition = strchr(buffer, ';');
//     if (!delimiterPosition) {
//         perror("Delimiter not found in the buffer");
//         fclose(file);
//         return;
//     }

//     *delimiterPosition = '\0';  // Replace delimiter with NULL to terminate the size string
//     char* sizeStr = buffer;  // Start of the size string just after prefix
//     int videoSize = atoi(sizeStr);  // Convert size string to integer

//     char* startOfVideoData = delimiterPosition + 1;  // Start of video data right after the delimiter
//     int videoDataInFirstBuffer = bytes_read - (startOfVideoData - buffer);  // Calculate the length of video data in the first buffer

//     // Write any video data from the first buffer
//     if (videoDataInFirstBuffer > 0) {
//         fwrite(startOfVideoData, 1, videoDataInFirstBuffer, file);
//     }

//     int chunk_count = 0;
//     int total_chunks = videoSize / BUF_SIZE + (videoSize % BUF_SIZE > 0);

//     printf("Total_chunks: %d\n", total_chunks);
//     // Continue reading the rest of the video data

//     int totalBytesRead = 0;
//     while (chunk_count < total_chunks) {
//         memset(buffer, 0, BUF_SIZE);
//         bytes_read = read(fd, buffer, BUF_SIZE);

//         if (bytes_read > 0) {
//             int bytesToWrite = bytes_read;
//             if (totalBytesRead + bytesToWrite > videoSize) {
//                 bytesToWrite = videoSize - totalBytesRead;  // Adjust the last chunk size
//             }
//             fwrite(buffer, 1, bytesToWrite, file);
//             printf("Finish writing chunk %d\n", chunk_count);
//             totalBytesRead += bytesToWrite;
//             chunk_count++;
//         } else if (bytes_read == 0) {
//             // EOF reached
//             printf("EOF reached after chunk %d\n", chunk_count);
//             break;
//         } else {
//             perror("Error reading video data");
//             break;
//         }
//     }

//     // while (chunk_count < total_chunks) {
//     //     memset(buffer, 0, BUF_SIZE);
//     //     // printf("Ready to read chunk %d\n", chunk_count);
//     //     bytes_read = read(fd, buffer, BUF_SIZE);
//     //     // printf("Finish reading chunk %d\n", chunk_count);
//     //     // if (bytes_read > 0) {
//     //         int bytesToWrite = bytes_read;
//     //         // if (totalBytesRead + bytesToWrite > videoSize) {
//     //         //     bytesToWrite = videoSize - totalBytesRead;  // Adjust the last chunk size
//     //         // }
//     //         // printf("Ready to write chunk %d\n", chunk_count);
//     //         fwrite(buffer, 1, bytesToWrite, file);
//     //         printf("Finish writing chunk %d\n", chunk_count);
//     //         // totalBytesRead += bytesToWrite;
//     //         chunk_count++;
//     //     // } else if (bytes_read < 0) {
//     //     //     perror("Error reading video data");
//     //     //     break;

//     // // }
//     // }
//     printf("Chunk_count: %d\n", chunk_count);
//     // fclose(file);
//     printf("[system] Video saved as received_video.mp4\n");
//     // Forward the received video to other clients

//     // for (int i = 0; i < SEM_SIZE; i++) {
//     //     if (cli_fd[i] != -1 /*&& cli_fd[i] != fd*/) {
//     //         write(cli_fd[i], "[VIDEO]", strlen("[VIDEO]"));

//     //         rewind(video_file);  // Reset file pointer for each client
//     //         while (!feof(video_file)) {
//     //             size_t read_bytes = fread(buffer, 1, BUF_SIZE, video_file);
//     //             if (read_bytes > 0) {
//     //                 write(cli_fd[i], buffer, read_bytes);
//     //             }
//     //             if (ferror(video_file)) {
//     //                 perror("Error while reading video file for forwarding");
//     //                 break;
//     //             }
//     //         }
//     //     }
//     // }
//     if (!file) {
//         perror("Failed to open video file for forwarding");
//         return;
//     }

//     // 获取视频文件的大小
//     // rewind(file);  // Reset file pointer to the beginning
//     fclose(file);

//     FILE* file2 = fopen("received_video.mp4", "rb");
//     rewind(file2);
// // Create a buffer for the video data
//     char *videoData = new char[BUF_SIZE_1];
//     int read_bytes = fread(videoData, 1, videoSize, file2);  // Read the entire video file into the buffer
//     fclose(file2);
//     printf("read_bytes: %d", read_bytes);
//     printf("videoSize: %d", videoSize);
//     if (read_bytes != videoSize) {
//         printf("Error: Not able to read the entire video file.\n");
//         delete[] videoData;
//         return;
//     }

//     // Prepare the prefix and size
//     std::string videoPrefix = "[VIDEO]";
//     std::string bytes_read_ch = std::to_string(videoSize);  // Convert video size to string
//     videoPrefix += bytes_read_ch + ";";

//     size_t prefixLength = videoPrefix.length();
//     size_t totalLength = prefixLength + videoSize;  // Total length = prefix + video data

//     // Allocate space for the concatenated data (prefix + video data)
//     char* concatenatedData = new char[totalLength + 1];  // +1 for null-terminator

//     // Copy the prefix into the buffer
//     memcpy(concatenatedData, videoPrefix.c_str(), prefixLength);

//     // Copy the video data into the buffer after the prefix
//     memcpy(concatenatedData + prefixLength, videoData, videoSize);


//     // Send the concatenated data to each client
//     for (int i = 0; i < SEM_SIZE; i++) {
//         if (cli_fd[i] > 0) {  // Ensure we have a valid socket descriptor
//             // Send the entire concatenated data (header + video data) to the client
//             ssize_t totalBytesSent = 0;
//             size_t chunkSize = BUF_SIZE;  // You can adjust the chunk size here

//             while (totalBytesSent < totalLength) {
//                 // size_t bytesToSend = (totalLength - totalBytesSent < chunkSize) ? (totalLength - totalBytesSent) : chunkSize;
//                 // ssize_t sent = write(cli_fd[i], concatenatedData + totalBytesSent, bytesToSend);

//                 ssize_t sent = write(cli_fd[i], concatenatedData + totalBytesSent, totalLength - totalBytesSent);
//                 if (sent == -1) {
//                     perror("Failed to send data to client");
//                     break;
//                 }

//                 totalBytesSent += sent;
//             }
//         }
//     }

//     // Cleanup
//     // delete[] buffer;
//     delete[] videoData;
//     delete[] concatenatedData;

//     printf("[system] Video forwarded to other clients\n");
// }
// void handle_video(int fd, char* clientBuf) {
//     // 打开文件以写入接收到的视频数据
//     FILE *file = fopen("received_video.mp4", "wb");
//     if (!file) {
//         perror("Failed to open file for writing");
//         return;
//     }



//     // 读取头部信息：格式为 [VIDEO]size;
//     //char header[128] = {0};
//     // int bytes_read = read(fd, buffer, BUF_SIZE);
//     // if (bytes_read <= 0) {
//     //     perror("Failed to read header from socket");
//     //     fclose(file);
//     //     return;
//     // }

//     // 解析头部信息
//     // buffer[bytes_read] = '\0';
//     // printf("Bytes_read: %d\n",bytes_read);
//     // printf("Buffer content: %s\n", buffer);

//     char *delimiterPosition = strchr(clientBuf, ';');
//     if (!delimiterPosition) {
//         perror("Delimiter not found in header");
//         fclose(file);
//         return;
//     }

//     *delimiterPosition = '\0';  // 替换分隔符为字符串结束符
//     char *sizeStr = clientBuf + 7;
//     int videoSize = atoi(sizeStr);  // 跳过 "[VIDEO]" 并转换大小字符串为整数

//     printf("Receiving video of size: %d bytes\n", videoSize);

//     char *startofVideoData = delimiterPosition + 1;
//     int videoDataInFirstBuffer = BUF_SIZE - (startofVideoData - clientBuf);

//     if(videoDataInFirstBuffer > 0){
//         fwrite(startofVideoData, 1, videoDataInFirstBuffer, file);
//     }


//     // 分配缓冲区以接收整个视频数据
//     // char *videoBuffer = (char *)malloc(videoSize);
//     // if (!videoBuffer) {
//     //     perror("Failed to allocate memory for video data");
//     //     fclose(file);
//     //     return;
//     // }

//     int totalBytesRead = videoDataInFirstBuffer;
//     while (totalBytesRead < videoSize) {
//        // bytes_read = read(fd, videoBuffer + totalBytesRead, videoSize - totalBytesRead);
//         char buffer[BUF_SIZE];
//         int bytes_read = read(fd, buffer, BUF_SIZE);
//         if (bytes_read <= 0) {
//             perror("Error reading video data");
//            // free(videoBuffer);
//            // fclose(file);
//             break;
//         }
//         fwrite(buffer, 1, bytes_read, file);
//         totalBytesRead += bytes_read;
//     }

//     printf("Total video data received: %d bytes\n", totalBytesRead);

//     // 将接收到的视频数据写入文件
//     fclose(file);
//     printf("[system] Video saved as received_video.mp4\n");



//     // 释放内存
//     printf("[system] Video forwarded to all clients\n");
// }

void handle_video(int fd, char *clientBuf, int recv_size) {
    // 打开文件以写入接收到的视频数据
    FILE *file = fopen("received_video.mp4", "wb");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    // 解析头部信息：格式为 [VIDEO]size;
    char *delimiterPosition = strchr(clientBuf, ';');
    if (!delimiterPosition) {
        perror("Delimiter not found in header");
        fclose(file);
        return;
    }

    *delimiterPosition = '\0';  // 将分隔符替换为字符串结束符
    char *sizeStr = clientBuf + 7;  // 跳过 "[VIDEO]" 并获取视频大小字符串
    int videoSize = atoi(sizeStr);

    printf("Receiving video of size: %d bytes\n", videoSize);

    // 计算头部之后的视频数据起始位置
    char *startOfVideoData = delimiterPosition + 1;
    int videoDataInFirstBuffer = BUF_SIZE - (startOfVideoData - clientBuf);

    // 写入头部之后的视频数据到文件
    if (videoDataInFirstBuffer > 0) {
        fwrite(startOfVideoData, 1, videoDataInFirstBuffer, file);
    }

    // 分配内存来存储整个视频数据，包括头部和视频数据
    char *videoBuffer = (char *)malloc(videoSize);
    if (!videoBuffer) {
        perror("Failed to allocate memory for video data");
        fclose(file);
        return;
    }

    // 将头部后的视频数据复制到 videoBuffer
    int prefixSize = 7 + log10(videoSize) + 2;
    memcpy(videoBuffer, startOfVideoData, recv_size - prefixSize);
    int totalBytesRead = recv_size - prefixSize;

    // 继续读取剩余的视频数据
    while (totalBytesRead < videoSize) {
        char buffer[BUF_SIZE];
        int bytes_read = read(fd, buffer, BUF_SIZE);
        if (bytes_read <= 0) {
            perror("Error reading video data");
            free(videoBuffer);
            fclose(file);
            return;
        }

        fwrite(buffer, 1, bytes_read, file);
        memcpy(videoBuffer + totalBytesRead, buffer, bytes_read);
        totalBytesRead += bytes_read;
        printf("Received: %d bytes, Total: %d/%d bytes\n", bytes_read, totalBytesRead, videoSize);
    }

    fclose(file);
    printf("[system] Video saved as received_video.mp4\n");

    // 构造完整的发送缓冲区（头部 + 视频数据）
    char header[128];
    snprintf(header, sizeof(header), "[VIDEO]%d;", videoSize);

    size_t headerLength = strlen(header);
    size_t totalLength = headerLength + videoSize;

    char *sendBuffer = (char *)malloc(totalLength);
    if (!sendBuffer) {
        perror("Failed to allocate memory for send buffer");
        free(videoBuffer);
        return;
    }

    // 复制头部和视频数据到发送缓冲区
    memcpy(sendBuffer, header, headerLength);
    memcpy(sendBuffer + headerLength, videoBuffer, videoSize);

    // 向所有客户端一次性发送完整数据
    for (int i = 0; i < SEM_SIZE; i++) {
        if (cli_fd[i] > 0) {
            printf("Forwarding video to client %d\n", cli_fd[i]);

            ssize_t bytesSent = write(cli_fd[i], sendBuffer, totalLength);
            if (bytesSent == -1) {
                perror("Failed to send video to client");
            } else {
                printf("Sent %zd bytes to client %d\n", bytesSent, cli_fd[i]);
            }
        }
    }

    // 清理内存
    free(videoBuffer);
    free(sendBuffer);

    printf("[system] Video forwarded to all clients\n");
}


// 字符串分割函数
void split(char *src, const char *separator, char **dest, int *num)
{
	char *pNext;
	// 记录分隔符数量
	int count = 0;
	// 原字符串为空
	if (src == NULL || strlen(src) == 0)
		return;
	// 未输入分隔符
	if (separator == NULL || strlen(separator) == 0)
		return;
	/*
		c语言string库中函数，
		声明：
		char *strtok(char *str, const char *delim)
		参数：
		str -- 要被分解成一组小字符串的字符串。
		delim -- 包含分隔符的 C 字符串。
		返回值：
		该函数返回被分解的第一个子字符串，如果没有可检索的字符串，则返回一个空指针。
	*/
	char *strtok(char *str, const char *delim);
	// 获得第一个由分隔符分割的字符串
	pNext = strtok(src, separator);
	while (pNext != NULL)
	{
		// 存入到目的字符串数组中
		*dest++ = pNext;
		++count;
		/*
			strtok()用来将字符串分割成一个个片段。参数s指向欲分割的字符串，参数delim则为分割字符串中包含的所有字符。
			当strtok()在参数s的字符串中发现参数delim中包涵的分割字符时,则会将该字符改为\0 字符。
			在第一次调用时，strtok()必需给予参数s字符串，往后的调用则将参数s设置成NULL。
			每次调用成功则返回指向被分割出片段的指针。
		*/
		pNext = strtok(NULL, separator);
	}
	*num = count;
}
 
// 群发函数

void *send_all(char *buf)
{
	for (int i = 0; i < SEM_SIZE; i++)
	{
		// 若值为-1，则没有此群友，表示已经退出或未被占有
		if (-1 != cli_fd[i])
		{
			printf("%s\n", buf);
			printf("send to %d\n", cli_fd[i]);
			write(cli_fd[i], buf, strlen(buf));
		}
	}

}
/**
 * 发送给指定的用户
 * char *buf 发送的消息
 * int fd 发给谁
 */
void *send_one(char *buf, int fd)
{
	printf("send to %d : %s\n",fd, buf);
	// printf("send to %d\n", fd);
	write(fd, buf, strlen(buf));
}
 
// 服务端接收函数
void *server(void *arg) {
  int fd = *(int *)arg;
  // char buf[BUF_SIZE];
  // char name[BUF_SIZE],
  char ts[BUF_SIZE];

  // 获取昵称
  read(fd, clients[fd].name, BUF_SIZE);
  clients[fd].client_fd = fd;

  printf("存放clients[fd].client_fd = %d\n", clients[fd].client_fd);
  sprintf(ts, "[TEXT][system]热烈欢迎 %s 进入群聊\n", clients[fd].name);
  send_all(ts);

  for (;;) {
    // 接收信息,无信息时将阻塞
    int recv_size = read(fd, clients[fd].buf, BUF_SIZE);

    printf("What did I recieved from clients: %s\n", clients[fd].buf);
    // 收到退出请求

    if (recv_size <= 0 || strncmp(clients[fd].buf, "[CMD]", 5) == 0) {
      sprintf(ts, "[system]欢送 %s 离开群聊\n", clients[fd].name);

      int index = 0;
      for (; index < SEM_SIZE; index++) {
        if (cli_fd[index] == fd) {
          cli_fd[index] = -1;
          break;
        }
      }

      // 群发XXX退出聊天室提示消息
      send_all(ts);

      // 群友退出，信号量+1
      int n;
      sem_post(&sem);
      sem_getvalue(&sem, &n);

      printf("[system] %s 离开群聊,群聊还剩%d人\n", clients[fd].name, SEM_SIZE - n);
      strcpy(clients[fd].buf, "exit_request");

      write(fd, clients[fd].buf, strlen(clients[fd].buf));
      close(fd);
      pthread_exit(NULL);
    }

    // 处理普通消息
    if (recv_size > 0) {
      // 单独发送或者群发
      // if (NULL != strstr(clients[fd].buf, "send")) { // 单独发
      //   char str[100];
      //   char *p[10] = {0};
      //   int num = 0, i;
      //   strcpy(str, clients[fd].buf);

      //   split(clients[fd].buf, ":", p, &num);
      //   for (i = 0; i < SEM_SIZE; i++) {
      //     if (NULL != strstr(clients[i].name, p[2])) {
      //       char msg[200];
      //       sprintf(msg, "[悄悄话]%s:%s", clients[fd].name, p[3]);
      //       send_one(msg, clients[i].client_fd);
      //     }
      //   }
      // } else {
          // 群发
		if(strncmp(clients[fd].buf, "[TEXT]", 6) == 0){
			const char *text_message = clients[fd].buf + 6;
			char buf[BUF_SIZE];
        	sprintf(buf, "[TEXT]%s: %s", clients[fd].name, text_message);
        	send_all(buf);
        	memset(buf, 0, BUF_SIZE);


		} else if(strncmp(clients[fd].buf, "[VIDEO]", 7) == 0){
			printf("[system]接收到视频数据\n");

			handle_video(fd, clients[fd].buf, recv_size);
		}

      // }
     // Reset client buffer
    memset(clients[fd].buf, 0, BUF_SIZE);
    }

  }
}

 
/**
 * quit
 */
void sigint(int signum)
{
	close(svr_fd);
	sem_destroy(&sem);
	printf("[system]服务器关闭\n");
	exit(0);
}
 
int main()
{
	signal(SIGINT, sigint);
	// 初始化信号量，群聊上限SEM_SIZE人
	sem_init(&sem, 0, SEM_SIZE);
 
	// 创建socket对象
	printf("[system]创建socket对象...\n");
	svr_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (0 > svr_fd)
	{
		perror("socket");
		return -1;
	}
 
 
	//端口复用函数：解决端口号被系统占用的情况
	int on = 1;
	int gg = setsockopt(svr_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	if(gg==-1)
	{
		perror("setsockopt");
		return -1;
	}
 
	// 准备通信地址(自己)
	printf("[system]准备通信地址...\n");
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6666);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	socklen_t addrlen = sizeof(addr);
 
	// 绑定socket对象与地址
	printf("[system]绑定socket对象与地址...\n");
	if (bind(svr_fd, (struct sockaddr *)&addr, addrlen))
	{
		perror("bind");
		return -1;
	}
 
	// 设置监听和排除数量
	printf("[system]设置监听");
	if (listen(svr_fd, 10))
	{
		perror("listen");
		return -1;
	}
 
	printf("[system]等待客户端链接...\n");
	// 将初始值置全为-1，表示该聊天位置没有人占领
	memset(cli_fd, -1, sizeof(cli_fd));
	for (;;)
	{
		int sem_num;
		sem_getvalue(&sem, &sem_num);
 
		// 找到没有人占领的聊天位
		int index = 0;
		while (-1 != cli_fd[index])
			index++;
		cli_fd[index] = accept(svr_fd, (struct sockaddr *)&addr, &addrlen);
 
		if (0 > cli_fd[index])
		{
			perror("accept");
			return -1;
		}
 
		char buf[BUF_SIZE];
		if (0 >= sem_num)
		{
			printf("[system]人数已满，%d号客户端链接失败\n", cli_fd[index]);
			sprintf(buf, "[TEXT][system]人数已满，客户端链接失败\n");
			write(cli_fd[index], buf, strlen(buf));
            memset(buf, 0, BUF_SIZE);
			close(cli_fd[index]);
			cli_fd[index] = -1;
		}
		else
		{
			sem_trywait(&sem);
			sem_getvalue(&sem, &sem_num);
			// char msg[BUF_SIZE] = {};
			printf("[system]%d号客户端链接成功,当前聊天人数%d人\n", cli_fd[index], SEM_SIZE - sem_num);
			sprintf(buf, "[TEXT][system]客户端链接成功,当前聊天人数%d人\n", SEM_SIZE - sem_num);
			write(cli_fd[index], buf, strlen(buf));
            memset(buf, 0, BUF_SIZE);
			// 创建线程客户端
			pthread_t tid;
			pthread_create(&tid, NULL, server, &cli_fd[index]);
            //pthread_join(tid, NULL);
		}

	}
}
