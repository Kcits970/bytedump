SRC_DIR=src

bytedump: ${SRC_DIR}/main.c ${SRC_DIR}/hex.c ${SRC_DIR}/base64.c
	gcc -o bytedump ${SRC_DIR}/*.c -I ./src
