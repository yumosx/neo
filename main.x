BEGIN{
    out("start a program");
}

filter {
  #const (
    first = packet[12, u8];
    udp   = packet[23, u16];
    arp   = 0x0806;
    ip    = 0x0800;
  );

  case first:
    =arp -> pass;
    =ip -> =udp -> drop;
    then -> puts("no match");
  end
}
