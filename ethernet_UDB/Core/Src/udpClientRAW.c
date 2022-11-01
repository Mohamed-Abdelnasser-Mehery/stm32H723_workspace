


#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

#include "stdio.h"
#include "string.h"

#include "udpClientRAW.h"


void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port);
static void udpClient_send(void);

struct udp_pcb *upcb;
char buffer[100];
int counter = 0;

extern TIM_HandleTypeDef htim1;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	udpClient_send();
}


/** IMPLEMENTATION FOR UDP CLIENT :   source:https://www.geeksforgeeks.org/udp-server-client-implementation-c/

1. Create UDP socket.
2. Send message to server.
3. Wait until response from server is received.
4. Process reply and go back to step 2, if necessary.
5. Close socket descriptor and exit.
*/


void udpClient_connect(void)
{
	err_t err;

	/* 1. Create a new UDP control block  */
	upcb = udp_new();

	/* Bind the block to module's IP and port */
	ip_addr_t myIPaddr;
	IP_ADDR4(&myIPaddr, 192, 168, 0, 123);
	udp_bind(upcb, &myIPaddr, 8);


	/* configure destination IP address and port */
	ip_addr_t DestIPaddr;
	IP_ADDR4(&DestIPaddr, 192, 168, 0, 8);
	err= udp_connect(upcb, &DestIPaddr, 7);

	if (err == ERR_OK)
	{
		/* 2. Send message to server */
		udpClient_send ();

		/* 3. Set a receive callback for the upcb */
		udp_recv(upcb, udp_receive_callback, NULL);
	}
}


///This function send data to USB server


static void udpClient_send(void)
{
  struct pbuf *txBuf;
  char data[100];

  int len = sprintf(data, "\nQUANTTRONIX UDP client message %d", counter);

  /* allocate pbuf from pool*/
  txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

  if (txBuf != NULL)
  {
    /* copy data to pbuf */
    pbuf_take(txBuf, data, len);

    /* send udp data */
    udp_send(upcb, txBuf);

    /* free pbuf */
    pbuf_free(txBuf);
  }
}

/// This function handle the input data from UDB server
void udp_receive_callback(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
{
	/* Copy the data from the pbuf */
	strncpy (buffer, (char *)p->payload, p->len);
	  struct pbuf *txBuf;
	  char data[70];

	  int len = sprintf(data, "\nQUANTTRONIX UDP client message %s", buffer);

	  /* allocate pbuf from pool*/
	  txBuf = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);

	  if (txBuf != NULL)
	  {
	    /* copy data to pbuf */
	    pbuf_take(txBuf, data, len);

	    /* send udp data */
	    udp_send(upcb, txBuf);

	    /* free pbuf */
	    pbuf_free(txBuf);
	  }
	/*increment message count */
	counter++;

	/* Free receive pbuf */
	pbuf_free(p);
}


