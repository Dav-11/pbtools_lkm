#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

#include "generated/address_book.h"

#define PORT 60001

typedef struct {
    int size;
    uint8_t encoded[128];
} message;

message *new_address_book() {

    uint8_t workspace[1024];

    message *enc = (message*)malloc(sizeof(message));
	memset(enc, 0, sizeof(message));

	struct address_book_address_book_t *address_book_p;
    struct address_book_person_t *person_p;
    struct address_book_person_phone_number_t *phone_number_p;

	address_book_p = address_book_address_book_new(&workspace[0], sizeof(workspace));

	/* Add one person to the address book. */
    assert(address_book_address_book_people_alloc(address_book_p, 1) == 0);
    person_p = &address_book_p->people.items_p[0];
    person_p->name_p = "Kalle Kula";
    person_p->id = 56;
    person_p->email_p = "kalle.kula@foobar.com";

    /* Add phone numbers. */
    assert(address_book_person_phones_alloc(person_p, 2) == 0);

    /* Home. */
    phone_number_p = &person_p->phones.items_p[0];
    phone_number_p->number_p = "+46701232345";
    phone_number_p->type = address_book_person_home_e;

    /* Work. */
    phone_number_p = &person_p->phones.items_p[1];
    phone_number_p->number_p = "+46999999999";
    phone_number_p->type = address_book_person_work_e;

    /* Encode the message. */
    enc->size = address_book_address_book_encode(address_book_p, enc->encoded, sizeof(enc->encoded));
    assert(enc->size == 75);

	return enc;
}

int main() {
    int sockfd;
    struct sockaddr_in serveraddr;

    // create msg
    message *msg = new_address_book();

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(1);
    }

    // Set up server address structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Localhost address
    serveraddr.sin_port = htons(PORT);
    memset(serveraddr.sin_zero, 0, sizeof(serveraddr.sin_zero));

    // Send the message
    if (sendto(sockfd, msg->encoded, sizeof(msg->encoded), 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("sendto");
        close(sockfd);
        exit(1);
    }

    printf("Message sent to localhost on port %d\n", PORT);

    // Close the socket (not required for sending, but good practice)
    close(sockfd);

    return 0;
}