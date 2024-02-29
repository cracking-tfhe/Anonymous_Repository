def convert(str_lst):
	lst = []
	for string in str_lst:
		if (string != ""):
			temp = int(string)
			lst.append(temp)
	return lst
	
with open('generated_key.txt', 'r') as fp:
	lines = fp.readlines()
	temp = lines[0].strip().split(" ")
	gk = convert(temp)
	
with open('secret_key.txt', 'r') as fp:
	lines = fp.readlines()
	temp = lines[0].strip().split(" ")
	sk = convert(temp)
	
n = 630 #key size

for i in range(n):
	if(gk[i] != sk[i]):
		print("Mismatch in bit: " + str(i + 1) + "\n")
