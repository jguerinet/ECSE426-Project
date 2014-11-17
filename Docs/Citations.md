#Citations#

Passive Localization Research

- Bistatic passive localization - three algorithms - maximum likelihood estimator (MLE), and two linear least squares variants.
>Lieckfeldt, D.; Jiaxi You; Timmermann, D., "Exploiting RF-Scatter: Human Localization with Bistatic Passive UHF RFID-Systems," Wireless and Mobile Computing, Networking and Communications, 2009. WIMOB 2009. IEEE International Conference on , vol., no., pp.179,184, 12-14 Oct. 2009
doi: 10.1109/WiMob.2009.39
keywords: {UHF radio propagation;electromagnetic wave scattering;indoor communication;least squares approximations;maximum likelihood estimation;radiofrequency identification;radiofrequency interference;RF propagation;RF-scatter;bistatic passive UHF RFID-system;human localization;indoor environment;linear least squares variant;maximum likelihood estimator;multipath interference;passive RFID-tags;received signal strength;signal scattering;spatial correlation;ubiquitous computing;wireless localization;Humans;Indoor environments;Maximum likelihood estimation;Passive RFID tags;RF signals;Radio frequency;Radiofrequency identification;Radiofrequency interference;Scattering;Ubiquitous computing;RFID;localization;measurement;passive;transceiver-free},
URL: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=5325198&isnumber=5325194   
Not part of citation: http://ieeexplore.ieee.org/xpl/articleDetails.jsp?reload=true&arnumber=5325198

- Looks like the different approaches can be grouped into TOA, TDOA, and DTOA
>Yuan Wang; Shaodan Ma; Chen, C.L.P., "TOA-Based Passive Localization in Quasi-Synchronous Networks," Communications Letters, IEEE , vol.18, no.4, pp.592,595, April 2014
doi: 10.1109/LCOMM.2014.021214.132662
Abstract: This letter addresses the problem of localization in a quasi-synchronous network using time-of-arrival (TOA) measurements. The object to be localized is passive, i.e., it is neither a transmitter nor a receiver. Unlike prior TOA-based algorithms, no perfect synchronization between the transceivers is assumed here. A two-step linear algorithm is proposed to jointly estimate the location of the passive object and the unknown time offset between the transceivers. The Bayesian Cramer-Rao lower bound (BCRB) for quasi-synchronous networks is then given for comparison. It is shown that the proposed algorithm can achieve the BCRB and significantly outperform the algorithms using time-difference-of-arrival (TDOA) and differential TOA (DTOA) measurements.
keywords: {time-of-arrival estimation;transceivers;BCRB;Bayesian Cramer-Rao lower bound;TDOA;TOA based algorithms;TOA based passive localization;TOA measurements;linear algorithm;passive object;quasisynchronous networks;time-difference-of-arrival;time-of-arrival measurements;Bayes methods;Estimation;Receivers;Synchronization;Transceivers;Transmitters;Vectors;BCRB;Localization;TOA},
URL: http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6746730&isnumber=6798887

- From Wikipedia
>Angle of arrival[edit]
Angle of arrival (AoA) is the angle from which a signal arrives at a receiver. AoA is usually determined by measuring the time difference of arrival (TDOA) between multiple antennas in a sensor array. In other receivers, it is determined by an array of highly directional sensors—the angle can be determined by which sensor received the signal. AoA is usually used with triangulation and a known base line to find the location relative to two anchor transmitters.

>Time of arrival[edit]
Time of arrival (ToA, also time of flight) is the amount of time a signal takes to propagate from transmitter to receiver. Because the signal propagation rate is constant and known (ignoring differences in mediums) the travel time of a signal can be used to directly calculate distance. Multiple measurements can be combined with trilateration and multilateration to find a location. This is the technique used by GPS. Systems which use ToA, generally require a complicated synchronization mechanism to maintain a reliable source of time for sensors (though this can be avoided in carefully designed systems by using repeaters to establish coupling[8]).

>The accuracy of the TOA based methods often suffers from massive multipath conditions in indoor localization, which is caused by the reflection and diffraction of the RF signal from objects (e.g., interior wall, doors or furniture) in the environment. However, it is possible to reduce the effect of multipath by applying temporal or spatial sparsity based techniques.[27] [28]