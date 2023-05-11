#ifndef MAVLINKCONF_H
#define MAVLINKCONF_H
#include "stdint.h"
const uint8_t MAVLinkCRCExtra[] = {50, 124, 137, 0, 237, // keep lines, lines are indexes
217,
104,
119,
117,
0,
0,      // id 10
89,
0,
0,
0,
0,
0,
0,
0,
0,
214,    // id 20
159,
220,
168,
24,
23,
170,
144,
67,
115,
39,    // id 30
246,
185,
104,
237,
244,
222,
212,
9,
254,
230,     // id 40
28,
28,
132,
221,
232,
11,
153,
41,
39,
78,     // id 50
196,
0,
0,
15,
3,
0,
0,
0,
0,
0,     // id 60
153,
183,
51,
59,
118,
148,
21,
0,
243,
124,    // id 70
0,
0,
38,
20,
158,
152,
143,
0,
0,
14,     // id 80
106,
49,
22,
143,
140,
5,
150,
0,
231,
183,    // id 90
63,
54,
47,
0,
0,
0,
0,
0,
0,
175,    // id 100
102,
158,
208,
56,
93,
138,
108,
32,
185,
84,    // id 110
34,
174,
124,
237,
4,
76,
128,
56,
116,
134,    // id 120
237,
203,
250,
87,
203,
220,
25,
226,
46,
29,    // id 130
223,
85,
6,
229,
203,
1,
195,
109,
168,
181,    // id 140
47,
72,
131,
127,
0,
103,
154,
178,
200,
134,    // id 150
219,
208,
188,
84,
22,
19,
21,
134,
0,
78,      // id 160
68,
189,
127,    // name="AHRS"
154,
21,     // name="HWSTATUS"
21,
144,
1,
234,
73,      // id 170
181,
22,
83,
167,
138,
234,
240,
47,     // name="AHRS2"
189,
52,     // id 180
174,
229,    // name="AHRS3"
85,
159,
186,
72,
0,
0,
0,
0,      // id 190
92,
36,
71,      // name="EKF_STATUS_REPORT"
98,
120,
0,
0,
0,
0,
134,    // id 200
205,
0,
0,
0,
0,
0,
0,
0,
0,
0,    // id 210
0,
0,
0,
69,
101,
50,
202,
17,
162,
34,    // id 220
71,
15,
0,
0,
208,
207,
0,
0,
0,
163,    // id 230
105,
151,
35,
150,
179,
0,
0,
0,
0,
0,    // id 240
90,
104,
85,
95,
130,
184,
81,
8,
204,
49,    // id 250
170,
44,
83,
46,
0,
71,
131,
187,
92,
146,
179,
12,
133,
49  ,
26,
193,
35,
14,
109,
59,
22,
0,
0,
0,
126,
18,
0,
0,
0,
70,
48,
123,
74,
99,
137,
210,
1,
20,
0,
251,
10,
0,
0,
0,
0,
0,
0,
0,
19,
217,    // id 300
243,
0,
0,
0,
0,
0,
0,
0,
0,
28,
95,
0,
0,
0,
0,
0,
0,
0,
0,
243,
88,
243,
78,
132,
0,
0,
0,
0,
0,
23,
91,
236,
231,
72,
225,
245,
0,
0,
199,
99,
0,
0,
0,
0,
0,
0,
0,
0,
0,
232,    // id 350
0,
0,
0,
0,
0,
0,
0,
0,
0,
11,
0,
0,
0,
0,
0,
0,
0,
0,
0,
75,
0,
0,
117,
0,
251,
0,
0,
0,
0,
232,
0,
0,
0,
0,
147,
132,
4,
8,
0,
156,
0,
0,
0,
0,
0,
0,
182,
0,
0,
110,    // id 400
183,
0,
0,
0,
0,
0,
0,
0,
0,
160,
106,
33,
77,
};

typedef struct {uint16_t id; uint8_t crc;} MAVLinkCRCExtra2Type;
const MAVLinkCRCExtra2Type MAVLinkCRCExtra2[] = {
    {9000, 113},
    {9005, 117},
    {10001, 209},
    {10002, 186},
    {10003, 4},
    {11000, 134},
    {11001, 15},
    {11002, 234},
    {11003, 64},
    {11010, 46},
    {11011, 106},
    {11020, 205},
    {11030, 144},
    {11031, 133},
    {11032, 85},
    {11033, 195},
    {11034, 79},
    {11035, 128},
    {11036, 177},
    {11037, 130},
    {11038, 47},
    {11039, 142},
    {12900, 114},
    {12901, 254},
    {12902, 140},
    {12903, 249},
    {12904, 77},
    {12905, 49},
    {12915, 94},
    {12918, 139},
    {12919, 7},
    {12920, 20},
    {42000, 227},
    {42001, 239},
    {50001, 246},
    {50002, 181},
    {50003, 62},
    {50004, 240},
    {50005, 152}
};

inline uint8_t getMAVLinkExtra(uint32_t msgid) {
    if(msgid < sizeof(MAVLinkCRCExtra)) {
        return MAVLinkCRCExtra[msgid];
    } else {
        const uint16_t cnt_extra2 = sizeof(MAVLinkCRCExtra2)/sizeof(MAVLinkCRCExtra2[0]);
        for(uint16_t i = 0; i < cnt_extra2; i++) {
            if(MAVLinkCRCExtra2[i].id == msgid) {
                return MAVLinkCRCExtra2[i].crc;
            }
        }
    }

    return 0;
}

#endif // MAVLINKCONF_H
