#include



"Error: could not contact otp_enc_d on port"
"Error: key ‘" myshortkey "’ is too short"
"otp_enc error: input contains bad characters"

if (connect(listenSocketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)))
{
  printf("Error: failed to connect\n");
  exit(1);
}

serverHostInfo = gethostbyname(localhost);
if (serverHostInfo == NULL)
{
  fprintf(stderr, "Error (client): No such host\n");
  exit(0)
}

memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
