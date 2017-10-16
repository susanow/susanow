
# [WIP] RouteBricks: Exploiting Parallelism To Scale Sftware Routers

- Mihai Dobrescu (EPFL Lausanne Switzerland)
- Norbert Egi (Lncaster University Nancaster, UK)
- Katerina Argyraki
- Byung-Gon Chun
- Kevin Fall
- Gianluca Lannaccone
- Allan Knies
- Maziar Manesh
- Sylvia Ratnasamy

ACM Symposium on Operating Systems Principles (SOSP) 2009 publicated

---
## ABSTRACT

- 近年のサーバ技術によってモチベートされ,
  ソフトウェアルータのスケール問題を再検討
- 一つのPCと並列化技術で高速なソフトウェアルータの提案をする
- 35Gbpsソフトウェアルータ
- このルータはサーバを追加することでliner-scaleする
- Click/Linux 環境に親しく, 既製品(Off-the-Shelf)を用いて開発する

**General Terms:**<br>
Design, Experimentation, Measurement, Performance

**Keyword:**<br>
Software Router, Multicore, Parallelism, Programmability

---
## 1. INTRODUCTION

- Recent NW-Appliance has focuse to high-perf for limited packet
- NW is more Sophisticated
	- Data-loss protection, Application Accelaration, IDS, etc..
	- More Programable, Extensible
- Special purpose NW Appliances/Middleboxes [1,8,13-15]
	- 1: Security Gateway
  - 8: Real-Time Traffic Intelligence
	- 13: Application Acceleration
	- 15: Data Loss Protection
- Academia and Industry have taken steps to enable such extend [9.17,18,40,41]
	- 9: NetFPGA
	- 17: Cisco IOS
	- 18: Juniper Open IP Solution Program
	- 40: OpenFlow
	- 41: API Design Challenges for Open Router Platforms
- Difficult: nesessary-extension involve high-speed-dplane
- Examples
	- 13: applicatin acceleration
	- 8: logging
	- 1: excription
	- 14: Intrusion detection
	- 21,36,39: variety of more forward-looking research proposal
- Current: High-Perf & Programmable are competeing goals
- Extreme Start Point:
	- Network-processor
		- Semi Specilized,
		- exsitting high-end, specialized debices & programable API
		- Program to Hardware
	- Software-Router
		- Optimize their packet-process
		- familiar with Hardware platforms
		- Promise greater extensibility:
			- data/control plane functionality can be modified
			- only software upgrade
		- commodity servers whould allow networks
		  many desirable properties of PC-based Ecosystem
- In this paper:
	- focus on one extreme end of the design spectrum
	- explore the feasibility of building high-speed routers using only PC
- There are multiple challenges in building Router out of PCs:
	- Look for Right-Programming-Model to exploit Router
	- What primitives should be exposed to Developers
- Focus Performance
	- PC-router scalability possibility against Special HW perf
	- Link/Port speed is 10Gbps is already
	- Carrier grade router range from 10Gbps up to 92Tbps
	- Software-Router range is 1-5Gbps, current


## 2. DESIGN PRINCIPLES

- Goal is to make networks easier to program
- N x R bps (N: nb ports, R: packet-process rate)
- Task classification
	1. route-lookup and class as packet process
	2. switching from input port to output port
- Switch Fabric & central scheduler
	- R = {1, 2.5, 10, 40}Gbps
	- N = {10 to 1000} ports
	- Example:
		- Edge Router {1Gbps x 360port} (Cisco 7600 Series Router )
		- Core Router {10Gbps x 4608ports} (Cisco Carrier Routing System)
- Design Principle:
	- router functionality be parallelized across multiple server
	- server can be met with existing or at least upcomming server model
	- Think 1PC server as HW-Router Line card
- Parallelism
	- across servers: pc-cluster
	- within servers: thread

## 3. PARALLELIZING ACROSS SERVERS

Purposes:
1. Provide a physical path connection input and output ports
2. Determine at each point in timer whici input gets to relay packet

Guarantees:
1. 100% throughput
2. fairness (fair share of the capacity)
3. avoids reordering packets

Limit
1. limited internal link rates
2. limited per-node processing rate
3. limited per-node fanout

## 4. PARALLELIZING WITHIN SERVERS

ServerSpec
- 2.8GHz core x 4
- L3 cache x 8
- Slot x 2 (2CPUs)
- PCIe1.1x8 x 2
- 2port10GNIC x 2

Exploiting Parallelism
- Multiqueueu
- share packet-processing among cores
	- pipeline approach


## 5. EVALUATION: SERVER PARALLELISM
## 6. THE RB4 PARALLEL ROUTER
## 7. RELATED WORK
## 8. DISCUSSION
## 9. CONCLUSIONS
## 10. REFERENCES



