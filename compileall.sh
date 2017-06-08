#!/bin/bash

#344 Program 4 compile
gcc opt_enc.c -o opt_enc
gcc opt_enc_d.c -o opt_enc_d
gcc opt_dec.c -o opt_dec
gcc opt_dec_d.c -o opt_dec_d
gcc keygen.c -o keygen
