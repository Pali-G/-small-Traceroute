#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char IPZ[25] = "";//Ziel IP Adresse
int ttl = 1;
int max_ttl = 30;
char nachricht[1024];

int main(int argc, char const *argv[]){
  
  //Eingabeüberprüfung 
  if (argc >= 2){
    strcpy(IPZ, argv[1]);
    printf("Routenverfolgung zu %s\n", IPZ);
  } else{
    printf("Bitte geben die eine IP-Adresse ein\n");
    return -1;
  }

  // meine Socketadresse
  struct sockaddr_in start_socketaddr;
  start_socketaddr.sin_family = AF_INET; //Typ der Socketadressierung festlegen
  start_socketaddr.sin_port = htons(33434); //Port festlegen: Standart Traceroute ports von 33434 bis 33534
  start_socketaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); //eigene IP

  //Ziel Socketadresse
  struct sockaddr_in ziel_socketaddr;
  ziel_socketaddr.sin_family = AF_INET;
  ziel_socketaddr.sin_port = htons(33434);
  ziel_socketaddr.sin_addr.s_addr = inet_addr(IPZ); // Ziel IP Adresse

  //Client Adresse
  struct sockaddr_in client_addr;
  socklen_t addrlen = sizeof(struct sockaddr_in);; //Speicherbereich zur Untescheidung zwischen sockaddr_in und sock_addr

  while(ttl <= max_ttl){
    //sockets
    int dgsocket = socket(PF_INET, SOCK_DGRAM, 0); //Anforderung des Sockets zum verschicken der Datenpackete
    if(dgsocket < 0){
      printf("Fehler dgsocket");
      return -2;
    }
    int statussocket = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP); //Anforderung des Sockets zum Empfangen der ICMP-Packete
    if(statussocket < 0){
      printf("Fehler statussocket");
      return -3;
    }
    int setttl = setsockopt(dgsocket, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); //setzt die TTL
    if(setttl < 0){
      printf("Fehler setttl");
      return -5;
    }

    //send und receive
    if (sendto(dgsocket, nachricht, sizeof(nachricht), 0, (struct sockaddr *)&ziel_socketaddr, sizeof(ziel_socketaddr)) < 0){
      printf("Fehler bei sendto");
      return -4;
    }
    if (recvfrom(statussocket, nachricht, sizeof(nachricht), 0, (struct sockaddr *)&client_addr, &addrlen) < 0){
      printf("Fehler bei recvfrom");
      return -6;
    }

    else{
      inet_ntop(AF_INET, &client_addr.sin_addr, nachricht, INET_ADDRSTRLEN); //wandelt IPv4-Adressen von binär zu text um
      printf("%d %s\t \n", ttl, nachricht);
      if(!strncmp(nachricht, IPZ, sizeof(IPZ)))
	break;
        ttl++;
    }
    
    
  }
}
