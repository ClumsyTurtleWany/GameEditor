
class ProtoParser():

    # ProtoParser 클래스의 생성자 함수입니다. 
	# 생성자 함수는 __init__이라는 이름으로 정의하며, 클래스 인스턴스가 만들어질 때 자동으로 실행됩니다. 
	# self 매개변수는 인스턴스 자신을 참조하는데 사용되며, 
	# 나머지 매개변수들은 인스턴스 속성에 값을 할당하기 위해 사용됩니다.
	def __init__(self, start_id, recv_prefix, send_prefix):
		#[] = 동적 배열
		self.recv_pkt = []		# 수신 패킷 목록
		self.send_pkt = []		# 송신 패킷 목록
		self.total_pkt = []		# 모든 패킷 목록
		self.stard_id = start_id
		self.id = start_id
		self.recv_prefix = recv_prefix
		self.send_prefix = send_prefix

	#함수 정의 라고 생각
	def parse_proto(self, path):
		f = open(path,'r')
		lines = f.readlines()

		# lines라는 리스트에 있는 각 라인에 대해 반복문을 실행합니다. 
		# 반복문 안에서 line이라는 변수가 현재 라인을 가리킵니다.
		for line in lines:
			# line이라는 변수가 가리키는 라인이 'message’로 시작하는지 검사합니다.
			if line.startswith('message')== False:
				continue

			# line이라는 변수가 가리키는 라인을 공백 문자를 기준으로 나눠 리스트 형태로 반환합니다. 
			# line.split()[0] : message 일태니까
			# 그리고 그 리스트의 첫 번째 원소(인덱스 1)를 가져와 대문자로 변환합니다. 
			# 변환된 문자열을 pkt_name이라는 변수에 저장합니다. 
			# 예를 들어, line이 "message C2S_LOGIN"이면 pkt_name은 "C2S_LOGIN"이 됩니다.
			pkt_name = line.split()[1].upper()
			if(pkt_name.startswith(self.recv_prefix)):
				self.recv_pkt.append(Packet(pkt_name,self.id))
			elif pkt_name.startswith(self.send_prefix):
				self.send_pkt.append(Packet(pkt_name,self.id))
			else :
				continue

			self.total_pkt.append(Packet(pkt_name,self.id))
			self.id += 1

		f.close()

class Packet:
	def __init__(self,name,id):
		self.name = name
		self.id = id
