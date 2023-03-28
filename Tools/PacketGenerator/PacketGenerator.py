
import argparse
import jinja2
import ProtoParser

def main():
	
	# argparse 모듈을 사용하여 커맨드 라인에서 인자값을 받기 위한 객체를 생성합니다. 
	# argparse 모듈은 사용자 친화적인 커맨드 라인 인터페이스를 만들기 쉽게 해주는 모듈입니다. 
	# description 인자에는 프로그램의 설명을 넣습니다.
	arg_parse = argparse.ArgumentParser(description = 'PacketGenerator')

	# arg_parser 객체에 '--path'라는 이름의 인자를 추가합니다. 
	# type 인자에는 인자값의 타입을 지정합니다. 
	# default 인자에는 인자값이 주어지지 않았을 때의 기본값을 지정합니다. 
	# help 인자에는 인자에 대한 도움말을 넣습니다.
	# `--` 는 일반적으로 긴 옵션 이름을 나타내는 데 사용됩니다. 
	# add_argument 설정시 '--'를 붙이는게 기본인거 같다.
	arg_parse.add_argument('--path',type=str, default='../../Common/protoc-3.17.1-win64/bin/protocol.proto',help='.proto path')
	arg_parse.add_argument('--output',type=str,default='TestPacketHandler',help='output file')
	arg_parse.add_argument('--name',type=str,default='testName',help='namespace')
	arg_parse.add_argument('--recv', type=str, default='C_', help = 'recv convention')
	arg_parse.add_argument('--send', type=str, default='S_', help = 'send convention')

	# 커맨드 라인에서 입력받은 인자값들을 파싱하여 args라는 변수에 저장합니다.
	args = arg_parse.parse_args()

	parser = ProtoParser.ProtoParser(1,args.recv,args.send)
	parser.parse_proto(args.path)

	# jinja2 : template engine
	# jinja2 모듈을 사용하여 템플릿 파일을 로드하기 위한 객체를 생성합니다. 
	# jinja2 모듈은 텍스트 기반의 템플릿 엔진으로, 동적으로 텍스트를 생성할 수 있게 해주는 모듈입니다. 
	# 'Templates'라는 폴더에 있는 템플릿 파일들을 로드합니다.
	# Templates 폴더에 있는 PacketHandler.h 파일 건드림 
	file_loader = jinja2.FileSystemLoader('Templates',encoding = 'CP949')

	# Environment 클래스는 템플릿을 로드하고 렌더링하는 데 사용됩니다
	# Environment 객체는 템플릿 로더, 필터, 테스트 등을 구성할 수 있습니다.
	# loader 인자에는 file_loader 객체를 넘겨줍니다.
	env = jinja2.Environment(loader = file_loader)

	# 템플릿 파일을 읽어서 Template 클래스의 객체를 반환하는 메서드입니다.
	#template = env.get_template('PacketHandler.h')
	template = env.get_template('PacketHandler.h')
	output = template.render(parser=parser, output=args.output, spacename = args.name)

	f=open(args.output+'.h','w+')
	f.write(output)
	f.close()

	print(output)
	return 

# `__name__`은 파이썬에서 내부적으로 사용되는 특별한 변수 이름입니다. 이 변수는 해당 모듈의 이름을 가지고 있습니다. 
# 즉, 현재 `.py` 파일의 이름을 가지고 있는 변수입니다. 만약 `example.py`라면 `example`이라는 문자열을 `__name__` 변수가 가지고 있게 됩니다.
# 만약 파이썬 파일을 직접 실행하는 경우에는 `__name__` 변수에 `__main__`이라는 값이 할당됩니다. 이를 통해 직접 실행된 파일인지 아니면 다른 파일에서 import된 모듈인지 구분할 수 있습니다.
# 예를 들어, `first.py`와 `second.py`가 있다고 할 때, `first.py`에서 `second.py`를 import해서 사용하고, `second.py`에서도 `first.py`를 import해서 사용한다고 했을 때 
# 각자를 import한다고 가정하면 `first.py`를 기준으로 프로그램을 실행하면 `first.py`의 `__name__`은 `"__main__"`이고, `second.py`의 `__name__`은 `"second"`가 들어가게 됩니다. 
# 반대로 `second.py`를 기준으로 프로그램을 실행하면 `first.py`의 `__name__`은 `"first"`이고, `second.py`의 `__name__`은 `"__main__"`이 됩니다.
if __name__ == '__main__':
	main()