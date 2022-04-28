| Meilenstein | bis   |                          |   |   |
|-------------|-------|--------------------------|---|---|
| 1           | 29.04 | bis Socket Kommunikation |   |   |
| 2           | 20.05 | bis Multiclientfähigkeit |   |   |
| 3           | 10.06 | bis Pub/Sub              |   |   |

MS1:

- Client-Server Kommunikation
- Client Seite :
  - socket
  - connect
  - send
  - recv
  - close
- Server Seite
  - socket
  - bind
  - listen
  - accept
  - send
  - recv

Wir implementieren einen Server! Also implementieren wir die Funktionen,
die zu der Server gehören und die Serverschleife:
![img.png](img.png)
(Quelle: https://openbook.rheinwerk-verlag.de/c_von_a_bis_z/bilderklein/klein25_002.gif)


# Adress Struktur Socket
- Internet Socket-Adressenstruktur beruht auf struct *sockaddr_in*
- Definiert in <netinet/in.h>

struct sockaddr_in\
{\
short sin_family;        *// AF_INET (Tag)*\
unsigned short sin_port; *// TCP bzw. UDP Portnummer*\
struct in_addr sin_addr; *// 32-Bit IP-Adresse*\
char sin_zero[8]\
};

#Byte Order
Quelle: Beej's Guide to Network Programming\
The thing is, everyone in the Internet world has generally agreed that if you want to represent the two-byte hex number, say b34f, you’ll store it in two sequential bytes b3 followed by 4f. Makes sense, and, as Wilford Brimley17 would tell you, it’s the Right Thing To Do. This number, stored with the big end first, is called Big-Endian.

Unfortunately, a few computers scattered here and there throughout the world, namely anything with an Intel or Intel-compatible processor, store the bytes reversed, so b34f would be stored in memory as the sequential bytes 4f followed by b3. This storage method is called Little-Endian.

But wait, I’m not done with terminology yet! The more-sane Big-Endian is also called Network Byte Order because that’s the order us network types like.

Your computer stores numbers in Host Byte Order. If it’s an Intel 80x86, Host Byte Order is Little-Endian. If it’s a Motorola 68k, Host Byte Order is Big-Endian. If it’s a PowerPC, Host Byte Order is… well, it depends!

A lot of times when you’re building packets or filling out data structures you’ll need to make sure your two- and four-byte numbers are in Network Byte Order. But how can you do this if you don’t know the native Host Byte Order?

Good news! You just get to assume the Host Byte Order isn’t right, and you always run the value through a function to set it to Network Byte Order. The function will do the magic conversion if it has to, and this way your code is portable to machines of differing endiannes

