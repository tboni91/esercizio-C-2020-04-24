/*
 * esercizio-C-2020-04-24.c
 *
 *  Created on: 24 apr 2020
 *      Author: utente
/
*/

/*
scrivere un programma in C:

è data la stringa di caratteri:

char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";

il processo padre crea un file output.txt (se il file esiste già, va troncato a zero e sovrascritto).

il processo padre tiene aperto il file output.txt, salvare il descrittore in una variabile fd

il processo padre crea un processo figlio: l'obiettivo finale è che il file output.txt contenga tutta la stringa text_to_write e che i due processi (processo padre e processo figlio) si dividano il lavoro di scrittura nel file. La scrittura sul file avviene in "parallelo".

il processo padre scrive la prima metà di text_to_write nel file, cominciando a scrivere dalla posizione iniziale del file.

il processo figlio scrive l'altra metà di text_to_write del file, cominciando a scrivere da dove il processo padre ha terminato di scrivere.

il processo padre attende la conclusione del processo figlio e poi termina.

 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <errno.h>


int main(int argc, char *argv[]) {

  char * file_name = "/home/utente/output.txt";  // per Debian
  //char * file_name = "fibonacci.txt";
  char * text_to_write = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. At ultrices mi tempus imperdiet nulla malesuada pellentesque elit. Non enim praesent elementum facilisis leo vel fringilla est ullamcorper. Non quam lacus suspendisse faucibus interdum. Metus vulputate eu scelerisque felis imperdiet. At ultrices mi tempus imperdiet nulla malesuada pellentesque. At tempor commodo ullamcorper a lacus vestibulum. Consequat semper viverra nam libero justo laoreet sit amet. Facilisis magna etiam tempor orci eu. Convallis tellus id interdum velit laoreet id donec ultrices. Interdum velit euismod in pellentesque massa placerat duis ultricies. Senectus et netus et malesuada fames. Pharetra vel turpis nunc eget lorem dolor. Nisi porta lorem mollis aliquam ut porttitor leo a. Euismod elementum nisi quis eleifend quam adipiscing vitae proin.";
  unsigned int text_len = strlen(text_to_write);
  unsigned int first_half_len, second_half_len;
  char * first_half;
  char * second_half;
  int res;
  off_t file_offset;

  first_half_len = text_len / 2;
  second_half_len = text_len - first_half_len;

  first_half = calloc(first_half_len + 1, sizeof(char));
  second_half = calloc(second_half_len + 1, sizeof(char));

  if (first_half == NULL || second_half == NULL) {
	  perror("calloc()");
	  exit(EXIT_FAILURE);
  }

  strncpy(first_half, text_to_write, first_half_len);
  strcat(first_half, "\0");

  strncpy(second_half, text_to_write + first_half_len, second_half_len);
  strcat(second_half, "\0");

  int fd = open(file_name, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

  if (fd == -1) {
		perror("open()");
		exit(EXIT_FAILURE);
  }

  switch(fork()) {
  	case 0:
  		// child process
  		file_offset = lseek(fd, first_half_len, SEEK_SET);

  		if (file_offset == -1) {
			perror("lseek()");
			exit(EXIT_FAILURE);
		}

  		res = write(fd, second_half, second_half_len);

		if (res == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
  		break;
  	case -1:
  		perror("fork()");
  		exit(EXIT_FAILURE);
  		break;
  	default:
  		file_offset = lseek(fd, 0, SEEK_SET);

  		if (file_offset == -1) {
			perror("lseek()");
			exit(EXIT_FAILURE);
		}

  		res = write(fd, first_half, first_half_len);

		if (res == -1) {
			perror("write()");
			exit(EXIT_FAILURE);
		}

  		wait(NULL);
  		break;
  }

  // close file
  if (close(fd) == -1) {
	  perror("close");
	  exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}

