#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

int tl, tr, bl, br, N, P;

//There are 3 types of processes in our domain
//1. The corner processes (TL, TR, BR, BL) : These send and receive from 2 of its neighbours
//2. The edge processes : These send and receive from 3 of its neighbours
//3. The inside processes : These send and receive from all 4 of its neighbours

//Throughout the code, we specify the directions UP, RIGHT, BOTTOM, LEFT as 0, 1, 2, 3 respectively

void direct_communication(int myrank, double subdomain[N][N], double rec_buffer[N][N]) {
	int i, j;
	MPI_Request request[N];
	MPI_Status status[N];

	if ( myrank == tl || myrank == tr || myrank == bl || myrank == br ) {
		if (myrank == tl) {
			//TL
			//send last column with tag 1 (sending to right)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//send last row with tag 2 (sending to bottom)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process 1 with tag 3 (process 1 sent to its left), save in its recv right
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process P with tag 0 (process P sent to its up), save in its recv down
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);
		}
		else if (myrank == tr) {
			//TR
			//send last row with tag 2 (sending to bottom)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//send first column with tag 3 (sending to left)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process (myrank-1) with tag 1 , save in its recv left 
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process myrank+P with tag 0 (process myrank+P sent to its up), save in its recv down
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);
		}
		else if (myrank == bl) {
			//BL
			//send first row with tag 0 (sending to up)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//send last column with tag 1 (sending to right)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process (myrank-P) with tag 2, save in its recv up 
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process myrank+1 with tag 3 (process myrank+1 sent to its left), save in its recv right
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);
		} 
		else {
			//BR
			//send first row with tag 0 (sending to up)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//send first column with tag 3 (sending to left)
			for (i = 0; i < N; i++) { 
				MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process (myrank-P) with tag 2 , save in its recv up 
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);

			//receive from process myrank-1 with tag 1 (process myrank-1 sent to its right), save in its recv left
			for (i = 0; i < N; i++) { 
				MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
			}
			MPI_Waitall(N, request, status);
		}
	}
	//case 2
	else if ( ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) || ((myrank % P) == 0 || (myrank % P) == P-1) ) {
		if ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) {
			//Edge rows
			if (myrank > tl && myrank < tr) {
				//Top edge row

				//send last column with tag 1 (sending to right)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send last row with tag 2 (sending to bottom)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send first column with tag 3 (sending to left)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

		
				//receive last col with tag 3 (from left of myrank+1), save in recvright
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);
				
			}
			else {
				//Bottom edge row
				//send first row with tag 0 (sending to up)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send last column with tag 1 (sending to right)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send first column with tag 3 (sending to left)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive last col with tag 3 (from left of myrank+1), save in recvright
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);
			}
		}
		else {
			//Edge columns
			if (myrank % P == 0) {
				//Left edge column
				//send first row with tag 0 (sending to up)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send last column with tag 1 (sending to right)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send last row with tag 2 (sending to bottom)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive last col with tag 3 (from left of myrank+1), save in recvright
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

			}
			else {
				//Right edge column
				//send first row with tag 0 (sending to up)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send last row with tag 2 (sending to bottom)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//send first column with tag 3 (sending to left)
				for (i = 0; i < N; i++) { 
					MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				for (i = 0; i < N; i++) { 
					MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
				}
				MPI_Waitall(N, request, status);

			}
		}
	}
	//case 3
	else {
		//send top row to upward process (myrank-P), tag 0
		for (i = 0; i < N; i++) { 
			MPI_Isend(&subdomain[0][i], 1, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);

		//send rightmost column to rightwards process (myrank+1), tag 1
		for (i = 0; i < N; i++) { 
			MPI_Isend(&subdomain[i][N-1], 1, MPI_DOUBLE, myrank+1, 1, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);

		//send bottom row to downward process (myrank+P), tag 2
		for (i = 0; i < N; i++) { 
			MPI_Isend(&subdomain[N-1][i], 1, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);

		//send leftmost column to leftwards process (myrank-1), tag 3
		for (i = 0; i < N; i++) { 
			MPI_Isend(&subdomain[i][0], 1, MPI_DOUBLE, myrank-1, 3, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);


		//receive from upwards process (myrank-P), tag 2 (as upwards process sent down with tag 2), save in recv[0]
		for (i = 0; i < N; i++) { 
			MPI_Irecv(&rec_buffer[0][i], 1, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);	

		//receive from rightwards process (myrank+1), tag 3 (as rightwards process sent left with tag 3), save in recv[1]
		for (i = 0; i < N; i++) { 
			MPI_Irecv(&rec_buffer[1][i], 1, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);	

		//receive from downwards process (myrank+P), tag 0 (as downwards process sent up with tag 0), save in recv[2]
		for (i = 0; i < N; i++) { 
			MPI_Irecv(&rec_buffer[2][i], 1, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);

		//receive from leftwards process (myrank-1), tag 1 (as leftwards process sent right with tag 1), save in recv[3]
		for (i = 0; i < N; i++) { 
			MPI_Irecv(&rec_buffer[3][i], 1, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request[i]);
		}
		MPI_Waitall(N, request, status);
	}

}

int pack_data(double in_buffer[N][N], double* out_buffer, int position, int dimension, int index) {
	//dimension: 0 represents row, 1 represents column
	//index: the index of the row/col to pack. index=0 for first row/col and N-1 for last row/col
	int posiiton = 0, i;
	
	int size;
	MPI_Type_size(MPI_DOUBLE, &size); //size of MPI_Double

	int out_size = N * size; //size of the packed output buffer

	if (dimension) {
		//column wise packing
		for (i = 0; i < N; i++) {
			MPI_Pack (&in_buffer[i][index], 1, MPI_DOUBLE, out_buffer, out_size, &position, MPI_COMM_WORLD);
		}
	}
	else {
		//row wise packing
		for (i = 0; i < N; i++) {
			MPI_Pack (&in_buffer[index][i], 1, MPI_DOUBLE, out_buffer, out_size, &position, MPI_COMM_WORLD);
		}	
	}
	return position;
}

void packed_communication(int myrank, double subdomain[N][N], double rec_buffer[N][N]) {
	int i, j;
	MPI_Request request;
	MPI_Status status;

	//pack_buffer: 4 x N dimensions
	double** pack_buffer = (double**)malloc(4 * sizeof(double*));
	for (i = 0; i < 4; i++) 
		pack_buffer[i] = (double*)malloc(N * sizeof(double));

	double** pack_rec_buffer = (double**)malloc(4 * sizeof(double*));
	for (i = 0; i < 4; i++) 
		pack_rec_buffer[i] = (double*)malloc(N * sizeof(double));


	int size;
	MPI_Type_size(MPI_DOUBLE, &size); //size of MPI_Double
	int out_size = N * size; //size of the packed output buffer

	int position[4] = {0};
	int rposition[4] = {0};
	
	if ( myrank == tl || myrank == tr || myrank == bl || myrank == br ) {
		if (myrank == tl) {
			//TL
			//send last column with tag 1 (sending to right)
			position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
			MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send last row with tag 2 (sending to bottom)
			position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
			MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process 1 with tag 3 (process 1 sent to its left), save in its recv right
			MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}

			//receive from process P with tag 0 (process P sent to its up), save in its recv down
			MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}
		}
		else if (myrank == tr) {
			//TR
			//send last row with tag 2 (sending to bottom)
			position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
			MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send first column with tag 3 (sending to left)
			position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
			MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-1) with tag 1 , save in its recv left 
			MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}

			//receive from process myrank+P with tag 0 (process myrank+P sent to its up), save in its recv down
			MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}

		}
		else if (myrank == bl) {
			//BL
			//send first row with tag 0 (sending to up)
			position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
			MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send last column with tag 1 (sending to right)
			position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
			MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-P) with tag 2, save in its recv up 
			MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}

			//receive from process myrank+1 with tag 3 (process myrank+1 sent to its left), save in its recv right
			MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}
		} 
		else {
			//BR
			//send first row with tag 0 (sending to up)
			position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
			MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send first column with tag 3 (sending to left)
			position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
			MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-P) with tag 2 , save in its recv up 
			MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}

			//receive from process myrank-1 with tag 1 (process myrank-1 sent to its right), save in its recv left
			MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			for (i = 0; i < N; i++) { 
				MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
			}
		}
	}

	
	//case 2
	else if ( ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) || ((myrank % P) == 0 || (myrank % P) == P-1) ) {
		if ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) {
			//Edge rows
			if (myrank > tl && myrank < tr) {
				//Top edge row

				//send last column with tag 1 (sending to right)
				position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
				MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last row with tag 2 (sending to bottom)
				position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
				MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send first column with tag 3 (sending to left)
				position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
				MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}
				
			}
			else {
				//Bottom edge row
				//send first row with tag 0 (sending to up)
				position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
				MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last column with tag 1 (sending to right)
				position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
				MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send first column with tag 3 (sending to left)
				position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
				MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}
		}
	}
		else {
			//Edge columns
			if (myrank % P == 0) {
				//Left edge column
				//send first row with tag 0 (sending to up)
				position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
				MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last column with tag 1 (sending to right)
				position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
				MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last row with tag 2 (sending to bottom)
				position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
				MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}
				
				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

			}
			else {
				//Right edge column
				//send first row with tag 0 (sending to up)
				position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
				MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last row with tag 2 (sending to bottom)
				position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
				MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send first column with tag 3 (sending to left)
				position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
				MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}

				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}
				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				for (i = 0; i < N; i++) { 
					MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
				}
			}
		}
	}
	//case 3
	else {
		//send top row to upward process (myrank-P), tag 0
		position[0] = pack_data(subdomain, pack_buffer[0], 0, 0, 0);
		MPI_Isend(pack_buffer[0], position[0], MPI_PACKED, myrank-P, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		//send rightmost column to rightwards process (myrank+1), tag 1
		position[1] = pack_data(subdomain, pack_buffer[1], 0, 1, N-1);
		MPI_Isend(pack_buffer[1], position[1], MPI_PACKED, myrank+1, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		//send bottom row to downward process (myrank+P), tag 2
		position[2] = pack_data(subdomain, pack_buffer[2], 0, 0, N-1);
		MPI_Isend(pack_buffer[2], position[2], MPI_PACKED, myrank+P, 2, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		//send leftmost column to leftwards process (myrank-1), tag 3
		position[3] = pack_data(subdomain, pack_buffer[3], 0, 1, 0);
		MPI_Isend(pack_buffer[3], position[3], MPI_PACKED, myrank-1, 3, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		//receive from upwards process (myrank-P), tag 2 (as upwards process sent down with tag 2), save in recv[0]
		MPI_Irecv(pack_rec_buffer[0], out_size, MPI_PACKED, myrank-P, 2, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		for (i = 0; i < N; i++) { 
			MPI_Unpack(pack_rec_buffer[0], out_size, &rposition[0], &rec_buffer[0][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
		}

		//receive from rightwards process (myrank+1), tag 3 (as rightwards process sent left with tag 3), save in recv[1]
		MPI_Irecv(pack_rec_buffer[1], out_size, MPI_PACKED, myrank+1, 3, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		for (i = 0; i < N; i++) { 
			MPI_Unpack(pack_rec_buffer[1], out_size, &rposition[1], &rec_buffer[1][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
		}	

		//receive from downwards process (myrank+P), tag 0 (as downwards process sent up with tag 0), save in recv[2]
		MPI_Irecv(pack_rec_buffer[2], out_size, MPI_PACKED, myrank+P, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		for (i = 0; i < N; i++) { 
			MPI_Unpack(pack_rec_buffer[2], out_size, &rposition[2], &rec_buffer[2][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
		}

		//receive from leftwards process (myrank-1), tag 1 (as leftwards process sent right with tag 1), save in recv[3]
		MPI_Irecv(pack_rec_buffer[3], out_size, MPI_PACKED, myrank-1, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		for (i = 0; i < N; i++) { 
			MPI_Unpack(pack_rec_buffer[3], out_size, &rposition[3], &rec_buffer[3][i], 1, MPI_DOUBLE, MPI_COMM_WORLD);
		}
	}
}

void derived_communication(int myrank, double subdomain[N][N], double rec_buffer[N][N]) {
	int count = N;
	int blocklength = 1;
	int stride = N;

	MPI_Datatype columntype;
	MPI_Type_vector(count, blocklength, stride, MPI_DOUBLE, &columntype);
	MPI_Type_commit(&columntype);

	int i, j;
	MPI_Request request;
	MPI_Status status;

	if ( myrank == tl || myrank == tr || myrank == bl || myrank == br ) {
		if (myrank == tl) {
			//TL
			//send last column with tag 1 (sending to right)
			MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send last row with tag 2 (sending to bottom)
			MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process 1 with tag 3 (process 1 sent to its left), save in its recv right
			MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process P with tag 0 (process P sent to its up), save in its recv down
			MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
		else if (myrank == tr) {
			//TR
			//send last row with tag 2 (sending to bottom)
			MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send first column with tag 3 (sending to left)
			MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-1) with tag 1 , save in its recv left 
			MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process myrank+P with tag 0 (process myrank+P sent to its up), save in its recv down
			MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
		else if (myrank == bl) {
			//BL
			//send first row with tag 0 (sending to up)
			MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send last column with tag 1 (sending to right)
			MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-P) with tag 2, save in its recv up 
			MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
			
			//receive from process myrank+1 with tag 3 (process myrank+1 sent to its left), save in its recv right
			MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		} 
		else {
			//BR
			//send first row with tag 0 (sending to up)
			MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//send first column with tag 3 (sending to left)
			MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process (myrank-P) with tag 2 , save in its recv up 
			MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);

			//receive from process myrank-1 with tag 1 (process myrank-1 sent to its right), save in its recv left
			MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
			MPI_Wait(&request, &status);
		}
	}
	
	//case 2
	else if ( ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) || ((myrank % P) == 0 || (myrank % P) == P-1) ) {
		if ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) {
			//Edge rows
			if (myrank > tl && myrank < tr) {
				//Top edge row
				//send last column with tag 1 (sending to right)
				MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send last row with tag 2 (sending to bottom)
				MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send first column with tag 3 (sending to left)
				MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
		
				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
			}
			else {
				//Bottom edge row
				//send first row with tag 0 (sending to up)
				MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send last column with tag 1 (sending to right)
				MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);

				//send first column with tag 3 (sending to left)
				MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
			}
		}
		else {
			//Edge columns
			if (myrank % P == 0) {
				//Left edge column
				//send first row with tag 0 (sending to up)
				MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send last column with tag 1 (sending to right)
				MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send last row with tag 2 (sending to bottom)
				MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive last col with tag 3 (from left of myrank+1), save in recvright
				MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
			}
			else {
				//Right edge column
				//send first row with tag 0 (sending to up)
				MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send last row with tag 2 (sending to bottom)
				MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//send first column with tag 3 (sending to left)
				MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first row with tag 2 (from down of myrank-P), save in recvup
				MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive last row with tag 0 (from up of myrank+P), save in recvdown
				MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
				
				//receive first col with tag 1 (from right of myrank-1), save in recvleft
				MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
				MPI_Wait(&request, &status);
			}
		}
	}
	//case 3
	else {
		//send top row to upward process (myrank-P), tag 0
		MPI_Isend(&subdomain[0][0], N, MPI_DOUBLE, myrank-P, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//send rightmost column to rightwards process (myrank+1), tag 1
		MPI_Isend(&subdomain[0][N-1], 1, columntype, myrank+1, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);

		//send bottom row to downward process (myrank+P), tag 2
		MPI_Isend(&subdomain[N-1][0], N, MPI_DOUBLE, myrank+P, 2, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//send leftmost column to leftwards process (myrank-1), tag 3
		MPI_Isend(&subdomain[0][0], 1, columntype, myrank-1, 3, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//receive from upwards process (myrank-P), tag 2 (as upwards process sent down with tag 2), save in recv[0]
		MPI_Irecv(&rec_buffer[0], N, MPI_DOUBLE, myrank-P, 2, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//receive from rightwards process (myrank+1), tag 3 (as rightwards process sent left with tag 3), save in recv[1]
		MPI_Irecv(&rec_buffer[1], N, MPI_DOUBLE, myrank+1, 3, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//receive from downwards process (myrank+P), tag 0 (as downwards process sent up with tag 0), save in recv[2]
		MPI_Irecv(&rec_buffer[2], N, MPI_DOUBLE, myrank+P, 0, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
		//receive from leftwards process (myrank-1), tag 1 (as leftwards process sent right with tag 1), save in recv[3]
		MPI_Irecv(&rec_buffer[3], N, MPI_DOUBLE, myrank-1, 1, MPI_COMM_WORLD, &request);
		MPI_Wait(&request, &status);
		
	}

	MPI_Type_free(&columntype);
}

double count(int i, int j,int N){
                    double cnt=0.0;
                    if (i-1>=0)
                        cnt+=(1.0);
                    if (j+1<N)
                         cnt+=(1.0);
                    if (i+1<N)
                         cnt+=(1.0);
                    if (j-1>=0)
                      cnt+=(1.0);

                    return cnt;
}

void computation(double rec_buffer[N][N], double t[N][N], double ans[N][N], int neighbour[4] ,int N){
        
           int i,j;
           
           for(i=0;i<N;i++)
              for(j=0;j<N;j++)
                   ans[i][j]=0;
           
           if (neighbour[0]==1){
                for(j=0;j<N;j++)
                  ans[0][j]+=rec_buffer[0][j];
           }
           if (neighbour[1]==1){
               for(i=0;i<N;i++)
                    ans[i][N-1]+=rec_buffer[1][i];
              

           }  
           if (neighbour[2]==1){
                 for(j=0;j<N;j++)
                    ans[N-1][j]+=rec_buffer[2][j];
           }
           if (neighbour[3]==1){
                for(i=0;i<N;i++)
                    ans[i][0]+=rec_buffer[3][i];
           }

           for(i=0;i<N;i++)
              for(j=0;j<N;j++){ //add temp[i][j] to its 4 neighbours
                    if (i-1>=0)
                        ans[i-1][j]+=t[i][j]; 
                    if (j+1<N)
                         ans[i][j+1]+=t[i][j];
                    if (i+1<N)
                         ans[i+1][j]+=t[i][j];
                    if (j-1>=0)
                      ans[i][j-1]+=t[i][j];   
              }             

          for(i=0;i<N;i++)
              for(j=0;j<N;j++){
                ans[i][j]/= count(i,j,N);
            }  

}


int main( int argc, char *argv[]) {
 
	int myrank, size; 
	double sTime, eTime, dir_time, dir_maxTime, pack_time, pack_maxTime, der_time, der_maxTime;

	int time_steps = atoi(argv[2]);

	MPI_Init(&argc, &argv);

	int N2 = atoi (argv[1]); //N2 specifies the number of data points in the process
	N = sqrt(N2);  //The number of rows and columns for the 2D decomposition of data points

	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	int P2 = size; //number of processes
	P = sqrt(P2); //The number of rows and columns for the 2D decomposition of processes
	
	//open the corresponding file
	//the file is of the format N^2, directtime, packtime, derviedtime
	FILE* fp;
	if (myrank == 0) {
		if (P2 == 16) {
			fp = fopen("data16.csv", "a");
		}
		else if (P2 == 36) {
			fp = fopen("data36.csv", "a");
		}
		else if (P2 == 49) {
			fp = fopen("data49.csv", "a");
		}
		else {
			fp = fopen("data64.csv", "a");
		}
	}
	
	int neighbour[4]={0,0,0,0};
    


    //case 1
  if ( myrank == tl || myrank == tr || myrank == bl || myrank == br ) {
    if (myrank == tl) {
      neighbour[1]=1;
      neighbour[2]=1;
      
    }
    else if (myrank == tr) {
      //TR
      neighbour[3]=1;
      neighbour[2]=1;
              
    }
    else if (myrank == bl) {
      //BL
      neighbour[0]=1;
      neighbour[1]=1;
     
    } 
    else {
      //BR
      neighbour[0]=1;
      neighbour[3]=1;
        
    }
  }
  //case 2
  else if ( ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) || ((myrank % P) == 0 || (myrank % P) == P-1) ) {
    if ((myrank > tl && myrank < tr) || (myrank > bl && myrank < br)) {
      //Edge rows
      if (myrank > tl && myrank < tr) {
        //Top edge row
          neighbour[1]=1;
          neighbour[2]=1;
          neighbour[3]=1;     
      }
      else {
        //Bottom edge row
          neighbour[1]=1;
          neighbour[0]=1;
          neighbour[3]=1;
    }
 }
    else {
      //Edge columns
      if (myrank % P == 0) {
        //Left edge column
          neighbour[1]=1;
          neighbour[2]=1;
          neighbour[0]=1;
      }
      else {
        //Right edge column
          neighbour[0]=1;
          neighbour[2]=1;
          neighbour[3]=1;      
      }
    }
  }
  //case 3
  else {
    neighbour[0]=1;
    neighbour[1]=1;
    neighbour[2]=1;
    neighbour[3]=1;  
  } 

    //double ans[N][4096],t[N][4096];
    double (*ans)[N] = malloc(sizeof(double[N][N]));
    double (*t)[N] = malloc(sizeof(double[N][N]));    
	int i, j ,k;

	//double subdomain[N][N]; //subdomain of data consisting of NxN double data points

	double (*dir_subdomain)[N] = malloc(sizeof(double[N][N]));
	double (*pack_subdomain)[N] = malloc(sizeof(double[N][N]));
	double (*der_subdomain)[N] = malloc(sizeof(double[N][N]));

	double temp;

	//populate the subdomains with random values (same for all the subdomains corresponding to the three methods)
	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++) {
			temp = rand(); // generate a random value 
			dir_subdomain[i][j] = temp;
			pack_subdomain[i][j] = temp;
			der_subdomain[i][j] = temp;
		}
	}
	
	//double rec_buffer[4][N]; //receive buffer for each of the processes. Has a size of N for each of the directions
	double (*dir_rec_buffer)[N] = malloc(sizeof(double[4][N]));	
	double (*pack_rec_buffer)[N] = malloc(sizeof(double[4][N]));
	double (*der_rec_buffer)[N] = malloc(sizeof(double[4][N]));

	for (i = 0; i < 4; i++) {
		for (j = 0; j < N; j++) {
			dir_rec_buffer[i][j] = -1;
			pack_rec_buffer[i][j] = -1;
			der_rec_buffer[i][j] = -1;
		}
	}	
	
	tl = 0, tr = P-1;
	bl = (P-1)*P, br = (P-1)*P + (P-1);

	sTime = MPI_Wtime();
	for (i = 0; i < time_steps; i++) {
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               t[j][k] = dir_subdomain[j][k];
           }
		}
		direct_communication(myrank, dir_subdomain, dir_rec_buffer);
		computation(dir_rec_buffer, t, ans, neighbour, N);
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               dir_subdomain[j][k] = ans[j][k];
           }
		}
	}
	eTime = MPI_Wtime();
	dir_time = eTime - sTime;
	MPI_Reduce (&dir_time, &dir_maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


	sTime = MPI_Wtime();
	for (i = 0; i < time_steps; i++) {
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               t[j][k] = pack_subdomain[j][k];
           }
		}
		packed_communication(myrank, pack_subdomain, pack_rec_buffer);
		computation(pack_rec_buffer, t, ans, neighbour, N);
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               pack_subdomain[j][k] = ans[j][k];
           }
		}
	}
	eTime = MPI_Wtime();
	pack_time = eTime - sTime;
	MPI_Reduce (&pack_time, &pack_maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);





	sTime = MPI_Wtime();
	for (i = 0; i < time_steps; i++) {
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               t[j][k] = der_subdomain[j][k];
           }
		}
		derived_communication(myrank, der_subdomain, der_rec_buffer);
		computation(der_rec_buffer, t, ans, neighbour, N);
		for (j = 0; j < N; j++) {
           for (k = 0; k < N; k++) {
               der_subdomain[j][k] = ans[j][k];
           }
		}
	}
	eTime = MPI_Wtime();
	der_time = eTime - sTime;
	MPI_Reduce (&der_time, &der_maxTime, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);



/*
	if (myrank == 0) {
		printf("%lf %lf %lf\n", dir_maxTime, pack_maxTime, der_maxTime);
	}*/

	if (myrank == 0) {
		fprintf(fp, "%d,%lf,%lf,%lf\n", N, dir_maxTime, pack_maxTime, der_maxTime);
		fclose(fp);
	}

	free(dir_subdomain);
	free(dir_rec_buffer);

	free(pack_subdomain);
	free(pack_rec_buffer);

	free(der_subdomain);
	free(der_rec_buffer);

	MPI_Finalize();

	return(0);
}
