extends ColorRect

var touches = {}
var keys = []

var enet: ENetConnection
var peer: ENetPacketPeer

var running: bool = true

var enet_mutex: Mutex
var enet_send_queue = []

func enet_thread():
	while running:
		enet_mutex.lock()
		for packet in enet_send_queue:
			peer.send(0, packet, peer.FLAG_RELIABLE)
		enet_send_queue = []
		enet_mutex.unlock()
		
		var event = enet.service(1)
		match event[0]:
			enet.EVENT_CONNECT:
				print("Connected to enet server")
			enet.EVENT_DISCONNECT:
				print("Disconnected from enet server")
			enet.EVENT_RECEIVE:
				var packet = peer.get_packet()
				var type = packet.decode_u8(0)
				
				match type:
					0x03:
						enet_mutex.lock()
						enet_mutex.unlock()

func create_enet(ip, port):
	enet = ENetConnection.new()
	if enet.create_host(32, 2, 0, 0):
		print("Error creating enet host")
		return
	enet.compress(ENetConnection.COMPRESS_RANGE_CODER)
	
	print("connecting to enet server on IP " + ip)
	peer = enet.connect_to_host(ip, port, 32, 0)
	
	enet_mutex = Mutex.new()
	
	var thread = Thread.new()
	thread.start(enet_thread)

func send_packet(packet: PackedByteArray):
	enet_mutex.lock()
	enet_send_queue.push_back(packet)
	enet_mutex.unlock()
