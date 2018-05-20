#define REGIONS_NR 10
#define COPY 0
#define PASTE 1
#define WAIT 2
#define SOCK_ADDRESS "./CLIPBOARD_SOCKET"


/**
 *  struct for messages needed info
 */

typedef struct Smessage{
	int region;
	int message_size;
	int order;
}Smessage;