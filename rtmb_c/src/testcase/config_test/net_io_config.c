/*******************************************************************************
 * Copyright (c) 2008 International Business Machines Corp.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *    Ajith Ramanath            - initial API and implementation
 *    Radhakrishnan Thangamuthu - initial API and implementation
 *    Mike Fulton               - initial API and implementation
 *******************************************************************************/
 
#include <config_test.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>

/*
 * Fills the IP address of the localhost
 */
int get_local_address(struct in_addr *local_addr) {
	char host[MAX_HOST_NAME_LEN];
	struct hostent *hp;

	strcpy(host,"localhost");

	/* -- Disabling hostname lookup intentionally.
	rc = gethostname(host, MAX_HOST_NAME_LEN - 1);

	host[MAX_HOST_NAME_LEN - 1] = '\0';

	if (rc != 0) {
		return -1;
	}
	*/

	hp = gethostbyname(host);

	if (!hp) {
		RTMB_printf(stderr, "error in host lookup:%s\n", hstrerror(
		        h_errno));
		return -1;
	}

	if (hp->h_addrtype == AF_INET && hp->h_addr_list[0]) {
		*local_addr = (*(struct in_addr *) hp->h_addr_list[0]);
		return 0;
	}

	return -1;
}

/*
 * Name    : msg_alloc_init()
 * Desc    : Allocates and inits the msg to be sent over the network.
 * Args    : msg      - pointer to the msg that needs allocation
 *	     msg_size - Size of the msg
 * Returns : msg - after alloc and init, else NULL.
 */
char *msg_alloc_init(char *msg, int msg_size) {
	int i;
	/* Alloc the msg buffer */
	msg = (char *) malloc(msg_size);
	if (msg == NULL) {
		RTMB_printf(stderr,
		        "*** Panic *** malloc() failed in msg_alloc_init()\n");
		abort();
	}
	memset(msg, 0, msg_size);
	/* Fill up the buffer with some chars */
	for (i = 0; i < msg_size - 1; i++) {
		msg[i] = 'a' + (i % 26);
	}
	return msg;
}

/*
 * Name    : net_server_impl()
 * Desc    : Server implementation.
 * Args    : args - arguments for the server.
 * Returns : NULL;
 */
void *net_server_impl(void *args) {
	int cli_sock = 0;
	char *msg = NULL, *rcv_msg = NULL;
	int msg_size = DEF_NETIO_SZ;
	int serv_sock = 0;
	int queue_backlog = 1;
	server_arguments_t *serv_args = (server_arguments_t *) args;
	int ret = 0, retry = 0, opt_val = 1;

	struct sockaddr_in client_det;
	struct sockaddr_in server_det;
	socklen_t csock_len = sizeof(client_det);
	struct timeval t;
	int i;

	memset(&client_det, 0, sizeof(client_det));

	/* Alloc and init the msg buffer */
	msg = (char *) msg_alloc_init(msg, msg_size);
	if (msg == NULL) {
		return (void *) NULL;
	}
	rcv_msg = (char *) malloc(30 * DEF_NETIO_SZ);
	if (rcv_msg == NULL) {
		RTMB_printf(stderr,
		        "*** Panic *** malloc() failed in net_server_impl()\n");
		abort();
	}

	/* Set the params for the server side stuff */
	server_det.sin_family = AF_INET;
	server_det.sin_addr.s_addr = INADDR_ANY;
	server_det.sin_port = htons(SERVER_PORT);
	memset(server_det.sin_zero, '\0', sizeof(server_det.sin_zero));

	/* Open server socket */
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) {
		RTMB_printf(stderr,
		        "net_serv_impl: Error creating server socket\n");
		if (msg != NULL) {
			free(msg);
		}
		if (rcv_msg != NULL) {
			free(rcv_msg);
		}
		return (void *) NULL;
	}
	t.tv_sec = 10;
	t.tv_usec = 0;

	if (setsockopt(serv_sock, SOL_SOCKET, SO_RCVTIMEO, &t,
	        sizeof(struct timeval)) != 0) {
		perror("net_server_impl");
		RTMB_printf(stderr,
		        "net_server_impl: Error while setting SO_RCVTIMEO"
			        " socket option\n");

		if (msg != NULL) {
			free(msg);
		}
		if (rcv_msg != NULL) {
			free(rcv_msg);
		}
		return (void *) NULL;
	}
	if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &opt_val,
	        sizeof(int)) != 0) {
		perror("net_server_impl");
		RTMB_printf(stderr,
		        "net_server_impl: Error while setting SO_REUSEADDR "
			        "socket option\n");
		if (msg != NULL) {
			free(msg);
		}
		if (rcv_msg != NULL) {
			free(rcv_msg);
		}
		return (void *) NULL;
	}

	/* Bind to the socket */
	if (bind(serv_sock, (struct sockaddr *) &server_det,
	        sizeof(struct sockaddr)) == -1) {
		RTMB_printf(stderr,
		        "net_server_impl: Error binding to the server socket\n");
		perror("net_server_impl");
		if (msg != NULL) {
			free(msg);
		}
		if (rcv_msg != NULL) {
			free(rcv_msg);
		}
		return (void *) NULL;
	}

	/* Listen */
	if (listen(serv_sock, queue_backlog) == -1) {
		RTMB_printf(stderr,
		        "net_server_impl: Error listening to the "
			        "server socket\n");
		if (msg != NULL) {
			free(msg);
		}
		if (rcv_msg != NULL) {
			free(rcv_msg);
		}
		return (void *) NULL;
	}
	/* Get into a loop to accept the client connection */
	while (serv_args->stay_alive != 0) {
		/* Accept if client pings */
		if ((cli_sock = accept(serv_sock,
		        (struct sockaddr *) &client_det, &csock_len)) == -1) {
			continue;
		} else {
			break;
		}
	}
	/* Connection is done. So, wait till we recv response */
	while (serv_args->stay_alive != 0) {
		char *buffer;
		int bytes_remaining;

		for (i = 0; i < serv_args -> iterations; i++) {
			buffer = rcv_msg;
			bytes_remaining = msg_size;

			/* receive the message */
			do {
				ret
				        = recv(cli_sock, buffer,
				                bytes_remaining, 0);
				retry = 0;
				if (ret == ERROR) {
					if (errno== EINTR) {
						retry = 1;
					}
				} else if (ret > 0 && ret < bytes_remaining) {
					buffer += ret;
					bytes_remaining -= ret;
					retry = 1;

#ifdef TRACE_MSG
					RTMB_printf( stdout, "client: "
						"received = %d   "
						"expected = %d "
						"get_next = %d \n",
						ret,
						bytes_remaining + ret,
						bytes_remaining);
					fflush(stdout);

				} else if ( ret> 0 && ret == bytes_remaining
					&& ret != msg_size) {
					RTMB_printf( stdout, "client:"
						" received = %d   "
						"expected = %d "
						"msg_size  = %d \n",
						ret,
						bytes_remaining,
						msg_size);
#endif
				}
			} while (retry != 0);

			if ((retry == 0) && (ret <= 0)) {
				close(cli_sock);
				if (msg != NULL) {
					free(msg);
				}
				if (rcv_msg != NULL) {
					free(rcv_msg);
				}

				return (void *) NULL;
			}

			/* Send the reponse back */
			if (send(cli_sock, rcv_msg, msg_size, 0) != msg_size) {
				close(cli_sock);
				if (msg != NULL) {
					free(msg);
				}
				if (rcv_msg != NULL) {
					free(rcv_msg);
				}

				return (void *) NULL;
			}

		}

		msg_size *= 2;
		free(rcv_msg);

		rcv_msg = calloc(sizeof(char), msg_size);

		if (rcv_msg == NULL) {
			RTMB_printf(stderr,
			        "*** Panic *** malloc() failed in"
				        " msg_alloc_init()\n");
			abort();
		}
	}

	if (msg != NULL) {
		free(msg);
	}
	if (rcv_msg != NULL) {
		free(rcv_msg);
	}
	close(cli_sock);
	close(serv_sock);

	fflush(stdout);
	return (void *) NULL;
}

/*
 * Name    : net_io_config()
 * Desc    : Determines the net IO rate on the system
 * Args    : args - Test args.
 * Returns : NULL;
 */

void *net_io_config(void *args) {
	/* Get the args passed from parent thread */
	thread_args_t *targs = (thread_args_t *) args;
	cmd_line_args *cmd_args = targs->cmd_args;
	benchmark_test *bmtest = targs->bmtest;
	result_table_t *res_table = targs->res_table;
	test_results_t *result = NULL;
	int retry = 0;
	int test_status;

	/* Other Local variables of this function */
	int loop = 0, done = 0, count = 0;
	struct timespec start_time, end_time;
	long long time_diff = 0;
	pthread_t server_tid = 0;
	struct sockaddr_in server_det;
	int cl_sock_fd = 0;
	server_arguments_t *serv_args = NULL;
	pthread_attr_t attr;

	/* Use this to set the timeout value for the socket */
	struct timeval t;

	char *msg = NULL;
	int msg_size = DEF_NETIO_SZ;
	int new_iters = bmtest->_iterations;
	double clock_accuracy;
	char retry_status = UNACCEPTABLE_DEVIATIONS;
	char print_flag = FALSE;

	/*
	 * Make sure both client and server threads are running
	 * the right sched policy and priority
	 */

	/* Keep server thread alive with this stay_alive argument variable */
	serv_args = (server_arguments_t *) malloc(sizeof(server_arguments_t));
	if (serv_args == (server_arguments_t *) NULL) {
		RTMB_printf(stderr, "net_io_config: malloc() failed\n");
		abort();
	}
	memset(serv_args, 0, sizeof(server_arguments_t));
	serv_args->stay_alive = 1;
	serv_args-> iterations = new_iters;
	pthread_attr_init(&attr);

	clock_accuracy = get_min_exec_time(res_table);
	if (set_pthreadattr_sched_param(&attr, SCHED_FIFO, HIGH_PRIO_VAL) < 0) {
		targs->ret = ERROR;
		return (void *) NULL;
	}

	/* Start the server thread and let the server get into accept() code */
	if (pthread_create(&server_tid, &attr, net_server_impl, serv_args)
	        != SUCCESS) {
		RTMB_printf(stderr,
		        "net_io_config: Error creating server thread\n");
		perror("net_io_config");
		server_tid = 0;
		cleanup(serv_args, server_tid, msg, cl_sock_fd);
		targs->ret = ERROR;
		return (void *) NULL;
	}

	/* Alloc and init the msg buffer */
	msg = (char *) msg_alloc_init(msg, msg_size);
	if (msg == NULL) {
		RTMB_printf(stderr, "net_io_config: malloc() failed\n");
		abort();
	}

	/* Set the params for the server side stuff */
	server_det.sin_family = AF_INET;

	if (get_local_address(&server_det.sin_addr) < 0) {
		RTMB_printf(stderr,
		        "net_io_config: unable to get local IP \n");
		server_tid = 0;
		cleanup(serv_args, server_tid, msg, cl_sock_fd);
		targs->ret = ERROR;
		return (void *) NULL;
	}

	server_det.sin_port = htons(SERVER_PORT);
	memset(server_det.sin_zero, '\0', sizeof(server_det.sin_zero));

	/* Create the socket fd */
	cl_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (cl_sock_fd == -1) {
		RTMB_printf(stderr,
		        "net_io_config: Error creating client socket\n");
		perror("net_io_config");
		cleanup(serv_args, server_tid, msg, cl_sock_fd);
		targs->ret = ERROR;
		return (void *) NULL;
	}

	/*
	 * We do not want the server thread to indefinitely wait.
	 * Set timeout to some safe value (10 secs).
	 */
	t.tv_sec = 10;
	t.tv_usec = 0;
	if (setsockopt(cl_sock_fd, SOL_SOCKET, SO_RCVTIMEO, (void *) &t,
	        sizeof(struct timeval)) != 0) {
		RTMB_printf(stderr,
		        "net_io_config: Error while setting socket option\n");
		cleanup(serv_args, server_tid, msg, cl_sock_fd);
		targs->ret = ERROR;
		return (void *) NULL;
	}

	/* give some time for the server thread to start */
	sleep(2);

	/* Connect to the server */
	while ((connect(cl_sock_fd, (struct sockaddr *) &server_det,
	        sizeof(server_det)) == -1) && (retry <= 10)) {
		do_nano_sleep(400LL * MS);
		retry++;
		RTMB_verbose_printf(stderr, cmd_args, 1,
		        "net_io_config: Retrying connect to the "
			        "server socket\n");
	}

	/* Connection done. Now, try exchanging some msgs with server */
	result = create_test_result(1, new_iters);
	if (result == NULL) {
		RTMB_printf(stderr,
		        "ERROR: Cannot allocate memory for test_results_t");
		RTMB_printf(stderr, " in net_io_config()\n");
		abort();
	}
	strcpy((char *) result->desc, "Network I/O configuration test");

	RTMB_verbose_printf(stdout, cmd_args, 1, "\nTest Report for %s:\n",
	        (char*) &result->desc);
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "========================================================="
		        "==\n");
	RTMB_verbose_printf(stdout, cmd_args, 1,
	        "\nnet_io_config : Total number of iterations = %d\n\n",
	        new_iters);

	while (!done) {
		int bytes_remaining;
		char* buffer;

		for (loop = 0; loop < new_iters; loop++) {
			int ret = SUCCESS, retry = 0;
			bytes_remaining = msg_size;
			buffer = msg;

			/* Record start time */
			if (get_cur_time(&start_time) == ERROR) {
				cleanup(serv_args, server_tid, msg, cl_sock_fd);
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}

			/* Send the msg */
			if (send(cl_sock_fd, msg, msg_size, 0) != msg_size) {
				cleanup(serv_args, server_tid, msg, cl_sock_fd);
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}

			/* Wait till you recv response */
			do {
				ret = recv(cl_sock_fd, buffer, bytes_remaining,
				        0);
				retry = 0;
				if (ret == ERROR) {
					if (errno== EINTR) {
						retry = 1;
					}
				} else if (ret > 0 && ret < bytes_remaining) {
					buffer += ret;
					bytes_remaining -= ret;
					retry = 1;

#ifdef TRACE_MSG
					RTMB_printf( stdout, "server: "
						"received = %d   "
						"expected = %d "
						"get_next = %d \n",
						ret,
						bytes_remaining + ret,
						bytes_remaining);

				} else if ( ret> 0 && ret == bytes_remaining
					&& ret != msg_size) {
					RTMB_printf( stdout, "server: "
						"received = %d   "
						"expected = %d "
						"msg_size  = %d \n",
						ret,
						bytes_remaining,
						msg_size);
#endif

				}
			} while (retry != 0);

			/*  Now, record end time */
			if (get_cur_time(&end_time) == ERROR) {
				cleanup(serv_args, server_tid, msg, cl_sock_fd);
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}

			/* If there was error other than EINTR, return fail */
			if ((retry == 0) && (ret == ERROR)) {
				cleanup(serv_args, server_tid, msg, cl_sock_fd);
				free_chain(result, 0);
				targs->ret = ERROR;
				return (void *) NULL;
			}

			/* Get the time difference of start and end times */
			time_diff = get_time_diff(start_time, end_time);
			RTMB_verbose_printf(stdout, cmd_args, 2,
			        "net_io_config: Difference between end"
				        " and start times "
				        "= %.3f us \n", MICROSEC(time_diff));

			fflush(stdout);

			add_entry(result, time_diff, 0);
		}

		if (IS_EXEC_TIME_GRT_THAN_CLK(result, clock_accuracy)) {
			print_flag = TRUE;

			/* Check against the computed median for this test */
			test_status = check_pass_criteria(result, cmd_args,
			        bmtest, 0);

			if (test_status == SUCCESS) {
				retry_status = ACCEPTABLE_DEVIATIONS;
				done = 1;
				break;
			} else {
				if (++count == bmtest->_threshold) {
					RTMB_printf(stderr,
					        "net_io_config: exceeded "
						        "maximum attempts \n");
					break;
				}
			}
		}

		if (print_flag) {
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        "\nnet_io_config: Retrying test ");
			RTMB_verbose_printf(stdout, cmd_args, 1,
			        " with bigger work quantum to get"
				        " lesser variance...\n");
		}
		/*
		 * measured times are not consistent so retry with
		 * larger buffer.
		 */
		free_chain(result, 0);
		if (msg) {
			free(msg);
		}
		msg_size = msg_size * MULTIPLIER_FOR_SUB_ITER;
		msg = (char *) msg_alloc_init(msg, (msg_size));

		if (msg == NULL) {
			abort();
		}
	}

	/*net  IO rate is determined*/
	result->opern_amount = msg_size;

	add_result_to_result_table2(res_table, result, NETIO_CONFIG, bmtest);

	fflush(stdout);
	/* Clean up and leave */
	cleanup(serv_args, server_tid, msg, cl_sock_fd);
	fflush(stdout);
	targs->ret = SUCCESS;
	return (void *) NULL;
}

/*
 * Name    : cleanup()
 * Desc    : free()s all the memory used by the test.
 * Args    : serv_args  - Arguments passed to server thread.
 *	     server_tid - tid of the server.
 *	     msg	- message that was being passed around.
 *	   cl_sock_fd - socket fd that was used for communication.
 * Returns : void
 */
void cleanup(server_arguments_t *serv_args, pthread_t server_tid, char *msg,
        int cl_sock_fd) {
	if (server_tid != 0) {
		serv_args->stay_alive = 0;
		fflush(stdout);
		if (cl_sock_fd != 0) {
			close(cl_sock_fd);
		}
		pthread_join(server_tid, (void **) NULL);
		fflush(stdout);
	}
	if (msg != NULL) {
		free(msg);
	}
	if (serv_args != NULL) {
		free(serv_args);
	}
}
