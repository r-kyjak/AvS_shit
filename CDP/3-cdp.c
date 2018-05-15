#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>

#include <arpa/inet.h>
#include <net/if.h>

#define		EXIT_ERROR		(1)
#define		SNAP_PAYLOAD_SIZE	(1492)

/* Format ramca 802 SNAP = 802.3 + 802.2 LLC + SNAP */
struct SNAPFrame
{
  unsigned char DstMAC[6];
  unsigned char SrcMAC[6];
  unsigned short Length;
  unsigned char DSAP;
  unsigned char SSAP;
  unsigned char Control;
  unsigned char OUI[3];
  unsigned short PID;
  char Payload[0];
} __attribute__ ((packed));

/* Format elementarneho TLV */
struct TLV
{
  unsigned short Type;
  unsigned short Length;
  char Value[0];
} __attribute__ ((packed));

/* Format CDP spravy */
struct CDPMessage
{
  unsigned char Version;
  unsigned char TTL;
  unsigned short CSum;
  struct TLV TLV[0];
} __attribute__ ((packed));

int
main (int argc, char *argv[])
{
  int Socket;
  struct sockaddr_ll Addr;
  struct packet_mreq Mreq;
  struct SNAPFrame *F;
  struct CDPMessage *C;
  struct TLV *T;

  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s <interface name>\n", argv[0]);
      exit (EXIT_ERROR);
    }

  /* Otvorime packet socket a pocuvame vsetky ramce formatu 802.2 LLC */
  Socket = socket (AF_PACKET, SOCK_RAW, htons (ETH_P_802_2));
  if (Socket == -1)
    {
      perror ("socket");
      exit (EXIT_ERROR);
    }

  /* Vyplnime struct sockaddr_ll, aby sme pocuvali na vhodnom rozhrani */
  memset (&Addr, 0, sizeof (Addr));
  Addr.sll_family = AF_PACKET;
  Addr.sll_protocol = htons (ETH_P_802_2);
  Addr.sll_ifindex = if_nametoindex (argv[1]);
  if (Addr.sll_ifindex == 0)
    {
      perror ("if_nametoindex");
      close (Socket);
      exit (EXIT_ERROR);
    }

  /* Zviazeme socket s rozhranim */
  if (bind (Socket, (struct sockaddr *) &Addr, sizeof (Addr)) == -1)
    {
      perror ("bind");
      close (Socket);
      exit (EXIT_ERROR);
    }

  /* Pripravime si strukturu s m-cast MAC adresou, na ktorej chceme pocuvat. */
  memset (&Mreq, 0, sizeof (Mreq));
  Mreq.mr_ifindex = Addr.sll_ifindex;
  Mreq.mr_type = PACKET_MR_MULTICAST;
  Mreq.mr_alen = 6;
  Mreq.mr_address[0] = 0x01;
  Mreq.mr_address[1] = 0x00;
  Mreq.mr_address[2] = 0x0c;
  Mreq.mr_address[3] = 0xcc;
  Mreq.mr_address[4] = 0xcc;
  Mreq.mr_address[5] = 0xcc;

  /* Zacneme pocuvat na multicastovej MAC adrese. */
  if (setsockopt
      (Socket, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &Mreq, sizeof (Mreq)) == -1)
    {
      perror ("setsockopt");
      close (Socket);
      exit (EXIT_ERROR);
    }

  /* Alokujeme pamat pre jeden ramec */
  F = malloc (sizeof (struct SNAPFrame) + SNAP_PAYLOAD_SIZE);
  if (F == NULL)
    {
      perror ("malloc");
      close (Socket);
      exit (EXIT_ERROR);
    }

  for (;;)
    {
      int Length;

      /* Vycistime pamat a nacitame ramec. */
      memset (F, 0, sizeof (struct SNAPFrame) + SNAP_PAYLOAD_SIZE);
      read (Socket, F, sizeof (struct SNAPFrame) + SNAP_PAYLOAD_SIZE);

      /* Seria kontrol, ci prijaty ramec obsahuje spravu CDP. */

      if (F->DSAP != 0xaa)
	continue;

      if (F->SSAP != 0xaa)
	continue;

      if (F->Control != 0x03)
	continue;

      if (((F->OUI[0] << 16) + (F->OUI[1] << 8) + (F->OUI[2])) != 0x00000c)
	continue;

      C = (struct CDPMessage *) F->Payload;

      if (C->Version != 2)
	continue;

      T = C->TLV;

      /* Spracovanu mame zatial iba hlavicku LLC+SNAP a hlavicku CDP */
      Length = 8 + sizeof (struct CDPMessage);

      printf ("%02hhx%02hhx.%02hhx%02hhx.%02hhx%02hhx: ",
	      F->SrcMAC[0],
	      F->SrcMAC[1],
	      F->SrcMAC[2], F->SrcMAC[3], F->SrcMAC[4], F->SrcMAC[5]);

      while (Length < ntohs (F->Length))
	{

	  /* Zapamatame si dlzku celeho TLV. Musi byt aspon T+L=4B */
	  unsigned short int TLVLen = ntohs (T->Length);
	  if (TLVLen < sizeof (struct TLV))
	    {
	      printf ("corrupt TLV.\n");
	      break;
	    }

	  /* CDP do dlzky zapocitava aj hlavicku TLV. Dlzka V je o to kratsia. */
	  unsigned short int VLen = TLVLen - sizeof (struct TLV);
	  char StringValue[VLen + 1];
	  memset (StringValue, '\0', VLen + 1);

	  switch (ntohs (T->Type))
	    {
	    case 0x0001:
	      strncpy (StringValue, T->Value, VLen);
	      printf ("name: %s, ", StringValue);
	      break;

	    case 0x0003:
	      strncpy (StringValue, T->Value, VLen);
	      printf ("port: %s, ", StringValue);
	      break;
	    }

	  Length += TLVLen;
	  T = (struct TLV *) (T->Value + VLen);
	}
      printf ("\n");
    }

  free (F);
  close (Socket);
  exit (EXIT_SUCCESS);
}
