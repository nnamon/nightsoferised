Session 1 - Intro to Packet Analysis 
=================================
This week, we take a break from system stuff. Instead, we will be focusing on network stuff, in particular, traffic analysis. We will take a broad look at the wireshark, a network protocol analyzer and how it can be used in analysis of packets capture. 

Objectives
----------
1. Wireshark 101 
2. Wireshark filters
3. File extraction


1. Wireshark 101
----------------
For this session, you will need Wireshark. You can download it from https://www.wireshark.org/#download.

Let's open one of the packet capture, located in guests/session1/babyCap/babyCap.pcapng. 

![wireshark interface][wireshark1]
We will briefly explore the various part of the interface of Wireshark, before looking at the content of the packet capture. 

1. "Packet List" Pane
This pane displays aall packets in the current packet capture file. Each packet is shown in a single line. In this pane, you can usually look at brief information such as the source, destination address, protocol and information about the packet content. 

2. "Packet Details" Pane
This shows the current packet in depth. You can look at the various headers. 

3. "Packet Bytes" Pane
This shows the data of the current packet in hex and ascii. 

One very useful trick in wireshark is the ability to follow a stream. Let's try with a http packet. Right click on any of the HTTP packet > Follow TCP Stream. This is a very useful way to visualize what the application layers will see. 
![tcp stream][stream]

Another useful tool is the expert info (Analyze > Expert Info). You can get a quick sense of what is happening in this pane. 
![expert info][expertinfo]


### babyCap
Alright. Let's look at the same packet capture (babyCap.pcapng) and try to analysis what is is happening. From the expert info, we saw a bunch of https request and finally some request to port 23. Let's look at what is happening on port 23 (right click > Find Frame). 

Noticed this is actually a capture of a telnet session. Let's follow the stream. 
![baby capture][babyCap]

From here, we can observe the entire telnet session. Notice some of the letter are repeated 
- ccoossmmoo
- eexxiitt

Can anyone guess why? 

### babyPortScan
Let's open one of the packet capture, located in guests/session1/babyPortScan/babyPortScan.pcapng. 

This is another simple exercise. You are given a packet capture of a port scanning activity. Try and see whether you can identify all the open ports on the targeted machine. Also, can you roughly guess what is the type of operating system running on the targeted machine (hint: Look at TTL values)? 

Spoiler alert: This is what I have done to the targeted machine. It is a TCP Connect scan from port 1 to 100 sequentially. 
```console
nmap -sT -r -p1-100 192.168.100.62 

Starting Nmap 6.47 ( http://nmap.org ) at 2016-01-23 17:11 SGT
Nmap scan report for 192.168.100.62
Host is up (0.00065s latency).
Not shown: 96 closed ports
PORT   STATE SERVICE
22/tcp open  ssh
23/tcp open  telnet
80/tcp open  http
99/tcp open  metagram

Nmap done: 1 IP address (1 host up) scanned in 0.05 seconds
```

2. Wireshark Filters
--------------------
As you can see, the packet capture can very large. Wiresharks provide filters for scoping down the packet capture. One simple way to scope down by protocol.

For instance,
- tcp
- udp
- dns 
- http 
- telnet 

You can also scope down by addresses or port 
- ip.addr == 192.168.100.63
- ip.src == 192.168.100.62
- ip.dst == 192.168.100.62
- tcp.port == 80
- udp.port == 53

And you can address it more complex expression by using the logical operation like 
- and &&
- or ||
- not \!


3. File extraction
------------------
Very often, files such as binaries and image files are transmitted over the network. Let's take a look at how to extract such items from the packet capture. 

### babyFile
In this exercise, let's open guests/session1/babyFile/babyFile.pcapng. This is a rather short packet capture. 

![wireshark interface][babyfile]

Noticed this is a simple web request. In this packet, the index page is retrieved from 192.168.100.62. A file, picture.png is also downloaded to the client computer. Let's follow the tcp stream and see what we observed. 

We will observed the following at the end of the tcp stream  

```console
.PNG

.
...
IHDR...............<.....PLTE......U..~....pHYs..........+.....-IDATX...1..0...X...r.o...<J.`i..>.?:.k.P.h.|......x..C4..F.i.z.0.._.A.$m.
.).)r...T..<.Aj..a.{Fc..6>F....'...........W~.1.D.Q.-B.2.N...a#.\.`..\..T.Z~..............%...1........u.&gt....&..i.\s......L.A.
.Ar.....@.`w....y.rvF....u.C..#.%..L...I.. I.....^...A...D!.v'K..F`b.\9.....mwI..t..:....?...7....K.|.d.*....IEND.B`.

89 50 4E 47 0D 0A 1A 0A 00 00 00 0D 49 48 44 52 00 00 00 C8 00 00 00 C8 01 03 00 00 00 97 96 3C DD 00 00 00 06 50 4C 54 45 FF FF FF 00 00 00 55 C2 D3 7E 00 00 00 09 70 48 59 73 00 00 0E C4 00 00 0E C4 01 95 2B 0E 1B 00 00 01 2D 49 44 41 54 58 C3 ED 96 31 92 86 30 08 85 C9 58 A4 CC 11 72 14 6F A6 EE CD 3C 4A 8E 60 69 E1 C0 3E C8 3F 3A BF 6B BD 50 C8 68 91 7C CD 1B C8 03 88 DE 78 E3 7F 43 34 D6 83 46 D9 69 CA 7A E0 30 E4 C0 5F D6 41 D6 24 6D CA 0D A7 29 0E 29 72 94 15 D0 54 B7 19 3C 1A 41 6A A3 92 61 1B 7B 46 63 11 AB 36 3E 46 A9 1F DE 81 27 F9 B8 C4 C8 A3 7F 1C C9 99 D7 B4 57 7E E8 31 9E 44 B6 51 AB 2D 42 7F 32 EA 4E F4 8E C6 61 23 93 5C D9 60 10 82 5C A2 DA 54 A0 5A 7E E4 D6 F9 9C 09 F4 1E A6 9D A9 2E B9 A5 EB 25 FA 13 CC 31 15 BE 8D E6 92 96 F6 EB 75 06 26 67 74 FF CC A1 08 26 1C 9E 69 D1 5C 73 96 85 2E 87 BB 13 4C 91 41 ED 0D 17 41 72 15 B9 9C 15 80 40 B8 60 77 11 BD 16 AD 79 18 72 76 46 A4 93 D5 D8 75 09 43 FA 9C 23 9D 25 BA BB 4C F7 19 EC 49 CE FD 20 49 BF AE 1C 87 D8 5E 85 1E 83 41 F2 BD 1C 44 21 A8 76 27 4B 16 0E 46 60 62 93 5C 39 14 E9 CD 0F AA 6D 77 49 12 88 74 97 F4 3A CF C8 E8 DD 3F 9E E4 8D 37 FC E3 17 BE 4B 81 7C 93 64 8A 2A 00 00 00 00 49 45 4E 44 AE 42 60 82

```

It appears to be a png image. A look up online inform us that png images start with "89 50 4E 47 0D 0A 1A 0A" and end with "49 45 4E 44 AE 42 60 82". So it is very likely it is a png images file. In fact, the ascii text already hinted us that it could be a png file. For more information of various file header, you can take a look at http://www.garykessler.net/library/file_sigs.html. 

So let's use wireshark to export this section out. Let select the section of bytes belonging to the png, right click and select "Export Selected Packet Bytes" and save to a directory. This reveal a qr code, when decoded is flag{3xtracti0n_c0mp3t3d}. 

Folks. That's all for this session... 

[//]: # (Images)
[wireshark1]: ./images/wireshark1.JPG
[stream]: ./images/stream.JPG
[babyfile]: ./images/wireshark_file.JPG
[expertinfo]: ./images/expertinfo.JPG
[babyCap]: ./images/babycap.JPG