| Meilenstein | bis   |                          |     |   |
|-------------|-------|--------------------------|-----|---|
| 1           | 29.04 | bis Socket Kommunikation | [x] |   |
| 2           | 20.05 | bis Multiclientfähigkeit | [ ] |   |
| 3           | 10.06 | bis Pub/Sub              | [ ] |   |

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
die zu dem Server gehören und die Serverschleife:
![img.png](img.png)\
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

# Byte Order
Quelle: Beej's Guide to Network Programming\
>The thing is, everyone in the Internet world has generally agreed that if you want to represent the two-byte hex number, say b34f, you’ll store it in two sequential bytes b3 followed by 4f. Makes sense, and, as Wilford Brimley17 would tell you, it’s the Right Thing To Do. This number, stored with the big end first, is called Big-Endian.

>Unfortunately, a few computers scattered here and there throughout the world, namely anything with an Intel or Intel-compatible processor, store the bytes reversed, so b34f would be stored in memory as the sequential bytes 4f followed by b3. This storage method is called Little-Endian.

>But wait, I’m not done with terminology yet! The more-sane Big-Endian is also called Network Byte Order because that’s the order us network types like.

>Your computer stores numbers in Host Byte Order. If it’s an Intel 80x86, Host Byte Order is Little-Endian. If it’s a Motorola 68k, Host Byte Order is Big-Endian. If it’s a PowerPC, Host Byte Order is… well, it depends!

>A lot of times when you’re building packets or filling out data structures you’ll need to make sure your two- and four-byte numbers are in Network Byte Order. But how can you do this if you don’t know the native Host Byte Order?

>Good news! You just get to assume the Host Byte Order isn’t right, and you always run the value through a function to set it to Network Byte Order. The function will do the magic conversion if it has to, and this way your code is portable to machines of differing endiannes



# Systemaufrufe und deren Parameter
## Socket erzeugen socket()

### **socket_fd= socket(domain, type, protocol)**
**socket_fd** ist der Filedeskriptor, den wir zurückbekommen und return Wert ist int\
Der Fildeskriptor wird zur Identifikation des Sockets benutzt\
**domain** = Protokollfamilie zB AF_INET ist für ipv4\
**type** = Typ des Sockets z.B SOCK_STREAM ist für TCP Kommunikation \
**protocol** = genauer spezifizieren, aber in der Regel auf 0 gesetzt (= Standardprotokoll)

## Binden einer Adresse an das Socket

### **result= bind(socket_fd, address, address_len);**

**result** ist mit return Wert int und ist 0 wenn das Binden erfolgreich war und -1 wenn fehlerhaft \
**socket_fd** ist der Filedeskriptor, den wir bei socket() zurückbekommen\
**address** ist die Adresse, an die das Socket gebunden werden soll und ist ein struct ( besteht aus IP Adresse Feld und Port Adresse Feld) ist in netinet/in.h definiert siehe oben\
**address_len** ist die Größe von address und wird oft mit sizeof berechnet

Wenn man die Adresse an das Socket binden will, muss man die Felder der Adresse setzen:

**struct  sockaddr_in server;** // Struct für die Adresseserver.\
**sin_familiy= AF_INET;** //für ipv4\
**server.sin_addr.s_addr= INADDR_ANY;** //alle erreichbare Hostadressen\
**server.sin_port= htons(5678);** // host byte order to network byte order



## Auf Verbindung hören listen()

### **ret= listen(socket_fd, backlog);**
**return Wert** ist ein int, 0 wenn OK, -1 Fehler\
**socket_fd** ist Filedeskriptor des Sockets, der auf Verbindungen hört\
**backlog** ist die Länge der Warteschlange für Clients*

## Verbindung akzeptieren

### **connection_fd= accept(socket_fd, client, client_len);**
**return Wert** ist ein Filedescriotor der angenommenen Verbindung = Verbindungsdeskriptor mit cfd häufig gekennzeichnet\
**socket_fd** Filedeskriptor des Sockets, von dem eine Verbindung entgegengenommen werden soll = Randezvous Descriptor\
**client** Pointer auf die Struktur mit Infos über Client ( struct vom Typ sockaddr_in)\
**client_len** Länge der Struktur vom 2.Paramenter


## Daten senden send() und Daten empfangen recv()

### **number_bytes= recv(connection_fd, buffer, number_bytes, flags)**
### **number_bytes= send(connection_fd, buffer, number_bytes, flags)**

**return Wert** bei beiden f()s ist die Anzahl an übertragenen Bytes\
**connection_fd** ist der Verbindungsdeskriptor von accept(siehe oben)\
**buffer** Buffer der sendet/der empfängt\
**number_bytes** Anzahl Bytes, die verschickt/empfangen werden sollen\
**flags** i.d.R. auf 0 gesetzt\

# Nützliches

**in-> key** An Arrow operator in C allows to access elements in Structures and Unions\
**Zeiger** Zeiger * sind Variablen, in denen man Speicheradressen speichert. 

**mmap** (void *address, size_t length, int protect, int flags, int filedes,
off_t offset) 
- address = gibt eine bevorzugte Startadresse für die Abbildung an.Wenn NULL -> Kernel übernimmt das
- length =  Anzahl der Bytes, die abgebildet werden sollen.
- protect = Zugriffsart eingegeben: z.B. PROT_READ = man kann lesen
- flags = die Flags steuern die map FUnktion. Wir haben MAP_SHARED, damit alle Prozesse damit arbeiten können
  - und wir haben auch anonymes Mapping.  Anonymes Mapping bedeutet, dass das Mapping mit keiner Datei verbunden ist
- offset = da startet das Mapping. Bei uns am Anfang: 0;
- filedes = Filedeskriptor, das gemappt werden soll\
  https://www.youtube.com/watch?v=rPV6b8BUwxM
- https://stackoverflow.com/questions/21311080/linux-shared-memory-shmget-vs-mmap
>mmap method is a little bit more restrictive then shmget, but easier to use.\
shmget is the old System V shared memory model and has the widest support.\
mmap/shm_open is the new POSIX way to do shared memory and is easier to use.\
If your OS permits the use of POSIX shared memory then I would suggest going with that.\

Some hints:

If you create your children via fork then mmap with MAP_ANONYMOUS | MAP_SHARED is by far the easiest way - just one call. MAP_ANONYMOUS is however a Linux extension not specified by POSIX.

**fork()** erstellt Kinderprozesse. Returnt 0 zu dem Kind und ProzessId vom Kind zum Elternprozess\
https://www.youtube.com/watch?v=ss1-REMJ9GA&t=0s
- fork() == 0 --> das ist ein Kindprozess

## Ausführen

- [ ] VPN  & Doocker an
- [ ] sich im Terminal mit ssh zu local host verbinden
- [ ] Programm starten
- [ ] telnet <IP-Adresse> 5678
- [ ] Kommandos eingeben

 include <stdio.h> --> printf etc\
include <stdlib.h> --> zur Umwandlung von Zahlen, für Speicherverwaltung und Ähnliches\
include <string.h> --> Funktionen für Strings\
include <errno.h> --> Fehlercode und Fehler behandeln\
include <sys/types.h> --> beiinhaltet ssize_t und size_t\
include <sys/socket.h> --> defines the sockaddr structure\
include <netinet/in.h> defines the sockaddr_in structure t\
include <netdb.h> 
include <arpa/inet.h> --> byte order Operationen\
include <unistd.h> --> erlaubt uns Systemaufrufe zu nutzen wie fork()\


# Meilensteil 2
  
  --> The fork way is pretty straighforward : the server socket (the one which accepts the connections) is in the main process, and each time it accepts a new client, it forks a whole new process just to monitor this new client : this new process will be dedicated to it. Since there's one process per client, we don't care if i/o operations are blocking or not.

# Meilenstein 3
- wir haben semaphore benutzt, um race conditions zu verhindern

## Semaphore

  **Struktur eines Programms**
W_ausschluss mit COUNTER = 1 initialisieren\
**<nicht_kritischer_Bereich>**\
W_ausschluss.**DOWN()**;\
**<kritischer_Bereich>**\
W_ausschluss.**UP()**;\
**<nicht_kritischer_Bereich>**
  
- mit UP geben wir den Prozess frei und er tritt in den nicht kritischen Bereich ein
- mit DOWN blockieren wir den Prozess und er tritt in den kritischen Bereich ein
  
- im kritischen Bereich darf nur der blockierte Prozess arbeiten und der andere Prozess ist im Warteraum
  ![image](https://user-images.githubusercontent.com/80696082/172784516-43032ed6-7e5c-4513-8bf9-731d933848df.png)
- DOWN: P-Operation - passeeren (passieren)
- UP:  V-Operation - vrijgeven (freigeben)
  
- die Variable has_exclusive_access dient zur Prüfung, ob der Prozess blockiert ist oder nicht:
  - has_exclusive_access = 0 --> down
  - has_exclusive_access = 1 --> up

  
### sem_init()
 - initializes the unnamed semaphore at the address pointed to by sem. 
 - The value argument specifies the initial value for the semaphore.
 - The pshared argument indicates whether this semaphore is to be shared between the threads of a process, or between processes.
    - value 0, then the semaphore is shared between the threads of a process, and should be located at some address that is visible to all threads (e.g., a global variable, or a variable allocated dynamically on the heap).
    - If pshared is nonzero, then the semaphore is shared between processes, and should be located in a region of shared memory
  
### semop()
https://man7.org/linux/man-pages/man2/semop.2.html


**int semop(int id, struct sembuf *ops, int count);**

**Effekt:**
- Der Systemaufruf semop() manipuliert Semaphorwerte durch eine Gruppe\
von P- und V-Operationen. Führt mindestens eine der P-Operationen zur Blockierung, \
so wird der Prozeß blockiert und vorerst keine der Operationen ausgeführt.
>Includes:
include<sys/ipc.h>\
include<sys/sem.h>\
include<sys/types.h>


**Parameter:**

**int id:**
Interner Identifikator für die Semaphorgruppe, wie von semget() geliefert.\
**struct sembuf * ops:**
Zeiger auf ein Feld mit Semaphoroperationen. \
Die Komponenten dieses Feldes sind vom Typ struct sembuf:

>struct sembuf{\
short sem_num;\
short sem_op;\
short sem_flg;\
}
- **sem_num:** Gruppeninterne Nummer des Semaphors, auf dem
  die Operation ausgeführt werden soll.
- **sem_op**: 
  - **sem_op>0:** addiere sem_op zum Semaphorwert und
  deblockiere alle wartenden Prozesse, die dann einer nach
  dem anderen erneut versuchen, ihre Semaphoroperation
  erfolgreich auszuführen.
  - **sem_op<0:** falls Semaphorwert + sem_op>=0,
  addiere sem_op zum Semaphorwert, sonst blockiere.
int count: Anzahl der Semaphore in der Gruppe.

**Rückgabe:**
Wert des Semaphors, auf den die letzte Semaphoroperation in ops zugreift, bevor diese Operation stattfand, oder -1 bei Fehler.

### semctl()
  ![image](https://user-images.githubusercontent.com/80696082/172786851-15ea3a5d-4290-430d-8827-ec9f40ca31ae.png)

  - aus der VL:
  // Anschließend wird der Semaphor auf 1 gesetzt\
marker[0] = 1;
  - semctl wird benutzt, um Steurungsfunktionen durchzuführen
  
  
## Message Queue
**int msgget(key_t key, int msgflg)**
- This system call creates or allocates a System V message queue
- first argument, key, recognizes the message queue.
- The second argument, shmflg, specifies the required message queue flag/s such as IPC_CREAT (creating message queue if not exists)

**int msgsnd(int msgid, const void** *msgp, **size_t msgsz, int msgflg)**
- This system call sends/appends a message into the message queue 
- msgid recognizes the message queue i.e., message queue identifier. 
- msgp, is the pointer to the message, sent to the caller, defined in the structure of the following form

**struct msgbuf {
long mtype;
char mtext[1];
};**
- mtype is used for communicating with different message types
- mtext is an array or other structure whose size is specified by msgsz (positive value).Default: zero size message, which is permitted. 
- msgsz, is the size of message (the message should end with a null character)
- msgflg, indicates certain flags such as IPC_NOWAIT (returns immediately when no message is found in queue or MSG_NOERROR (truncates message text, if more than msgsz bytes)

**int msgrcv(int msgid, const** void *msgp,**size_t msgsz, long msgtype, int msgflg)**

- This system call retrieves the message from the message queue
- msgid, recognizes the message queue i.e., the message queue identifier 
- msgp, is the pointer of the message received from the caller. 
- msgsz, is the size of the message received
- msgtype, indicates the type of message − 
  - If msgtype is 0 − Reads the first received message in the queue 
  - If msgtype is +ve − Reads the first message in the queue of type msgtype (if msgtype is 10, then reads only the first message of type 10 even though other types may be in the queue at the beginning)
  - If msgtype is –ve − Reads the first message of lowest type less than or equal to the absolute value of message type (say, if msgtype is -5, then it reads first message of type less than 5 i.e., message type from 1 to 5)
- msgflg, indicates certain flags such as IPC_NOWAIT (returns immediately when no message is found in the queue or MSG_NOERROR (truncates the message text if more than msgsz bytes)
