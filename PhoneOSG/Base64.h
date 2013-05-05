#pragma once

void init_ords();
void base64_encode(char *in, const int in_len, char *out, int out_len);
void base64_decode(char *in, const int in_len, char *out, int *out_len);
void write_to_binary_file(char * out, int & count, const char* outfile);
void read_from_binary_file(char * in, int & count, const char* infile);