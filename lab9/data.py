import os
import re

def main():
	# stream = os.popen("tcpdump -nn -tt -r tcp-large-transfer-0-0.pcap >> rawdata")
	fin = open("pcap.txt")
	fout_re = open("seq2-re.data", "w")
	fout_no = open("seq1-normal.data", "w")
	lines = fin.readlines()
	pattern = re.compile(r'\d+.+seq\s\d+')
	time = re.compile(r'\d+\.\d+')
	seq = re.compile(r'\d+')
	old_seq = 0
	print len(lines)
	# genereate retransmission.data and normal.data
	# plot "normal.cwnd" using 1:2 title 'normal' with linespoints, "retransmission.cwnd" using 1:2 title 'retransmission' 
	for line in lines:
		res = pattern.findall(line)
		if len(res) == 1:
			new_seq = int(seq.findall(res[0])[-1])
			time_val = float(time.findall(res[0])[0])
			if new_seq < old_seq:
				# write into retransmission
				fout_re.write(str(time_val) + " " +str(new_seq) + "\n")
			else:
				# write into normal
				fout_no.write(str(time_val) + " " +str(new_seq) + "\n")
			old_seq = new_seq
	# generate slowstart.cwnd and congection_avoid.cwnd, 
	# plot "congection_avoid.cwnd" using 1:2 title 'congection_avoid' with linespoints, "slowstart.cwnd" using 1:2 title 'slowstart'
	fin_cwnd = open("project.cwnd")
	fout_sl_cwnd = open("slowstart.cwnd", "w")
	fout_ca_cwnd = open("congestionavoid.cwnd", "w") #congection avoid
	lines_cwnd = fin_cwnd.readlines()
	old_cwnd = 0
	for line_cwnd in lines_cwnd:
		nodes = line_cwnd.split("\t")
		print line_cwnd
		time_cwnd = nodes[0]
		new_cwnd = int(nodes[1])
		if new_cwnd == 2*old_cwnd or new_cwnd == 0:
			fout_sl_cwnd.write(time_cwnd + " " + str(new_cwnd) + "\n")
		else:
			fout_ca_cwnd.write(time_cwnd + " " + str(new_cwnd) + "\n")
		old_cwnd = new_cwnd



if __name__ == "__main__":
    main()
