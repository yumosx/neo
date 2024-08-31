use xdp;

#const (
    first = packet[12, u8];
    udp   = packet[23, u16];
    arp   = 0x0806;
    ip    = 0x0800;
  );

first?
    =arp -> pass;
    =ip -> udp? 
            =drop -> pass;
    then -> puts("no match");
end
