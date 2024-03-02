#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>

#include "generated/address_book.h"

#define PORT 4445

// Define your struct
typedef struct {
    int size;
    uint8_t encoded[128];
} encoded_data;

encoded_data *new_address_book() {

    uint8_t workspace[1024];

	encoded_data *enc = (encoded_data*)malloc(sizeof(encoded_data));
	memset(enc, 0, sizeof(encoded_data));

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

    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    // Connect to server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Prepare struct to send
    encoded_data *dataToSend = new_address_book();

	// string
	char example_string[50];
    strcpy(example_string, "Esempio!");

    // Send struct
    if (send(sockfd, example_string, sizeof(example_string), 0) == -1) {
        perror("send");
        close(sockfd);
		free(dataToSend);
        exit(EXIT_FAILURE);
    }

    printf("Data sent successfully\n");

    // Close socket
    close(sockfd);

    return 0;
}