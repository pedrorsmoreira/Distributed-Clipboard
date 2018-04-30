#define INBOUND_FIFO "INBOUND_FIFO"
#define OUTBOUND_FIFO "OUTBOUND_FIFO"
#define REGIONS_NR 10
#define MESSAGE_SIZE 100
#define COPY 0
#define PASTE 1
#define DATA_SIZE sizeof(Smessage)
#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"

#include <sys/types.h>

typedef struct Smessage{
	int region;
	int message_size;
	//char message[MESSAGE_SIZE];
	int order;
}Smessage;

int clipboard_connect(char * clipboard_dir);
int clipboard_copy(int clipboard_id, int region, void *buf, size_t count);
int clipboard_paste(int clipboard_id, int region, void *buf, size_t count);
int clipboard_wait(int clipboard_id, int region, void *buf, size_t count);
void clipboard_close(int clipboard_id);