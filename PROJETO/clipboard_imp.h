#define REGIONS_NR 10
#define COPY 0
#define PASTE 1
#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"

#include <sys/types.h>

typedef struct Smessage{
	int region;
	int message_size;
	int order;
}Smessage;