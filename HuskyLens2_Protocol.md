# Release Note
| Version | Date  | Publisher  | E-mail | Description |
| :--: | :----: | :--: | :----: | :----: |
| V0.1 | 2025-11-07  | ouki.wang  | ouki.wang@dfrobot.com | First version |
| V0.2 | 2025-11-07  | ouki.wang  | ouki.wang@dfrobot.com | 1. Add API for retrieving read/write algorithm parameters<br/>
2. Remove RETURN_OK and uniformly use RETURN_ARGS |

## General Command/Response
| Head55 | HeadAA  | CMD  | Data Length | Data | Checksum |
| :--: | :----: | :--: | :----: | :----: | :----: |
| 0x55 | 0xAA | 1 byte  | 1 byte | 0-n bytes | 1 byte|

## Algorithm ID
| ID | Algorithm  | Description  |
| :--: | :----: | :--: |
| 0 | Universal ID（None） | For handshakes, algorithm switching, and other general-purpose commands  |
| 1 | FACE_RECOGNITION | Contain facial feature data |
| 2 | OBJECT_TRACKING |  |
| 3 | OBJECT_RECOGNITION |  |
| 4 | LINE_TRACKING |  |
| 5 | COLOR_RECOGNITION |  |
| 6 | TAG_RECOGNITION |  |
| 7 | SELF_LEARNING_CLASSIFICATION |  |
| 8 | OCR_RECOGNITION |  |
| 9 | LICENSE_RECOGNITION |  |
| 10 | QRCODE_RECOGNITION |  |
| 11 | BARCODE_RECOGNITION |  |
| 12 | EMOTION_RECOGNITION |  |
| 13 | POSE_RECOGNITION | Contain human body key point data |
| 14 | HAND_RECOGNITION | Contain finger key point data |
| 15 | OBJECT_CLASSIFICATION |  |
| 16 | RFU |  |
| 17 | GAZE_RECOGNITION |  |
| 18 | RFU |  |
| 19 | FALLDOWN_RECOGNITION |  |
| 20 | SEGMENT |  |
| 21 | RFU |  |
| ... | ... |  |
| 100-127 | RESERVE |  |
| 128-255| User installed model |  |

## ID
| ID | Meaning  | Description  |
| :--: | :----: | :--: |
| 0 | Unlearned ID | In the line-following application, the default ID is 0. If multiple colored tracks are learned, the ID represents the color ID being learned.  |
| 1-n | Learned ID |   |

## clolrID
| ID | COLOR  |  RGB |
| :--: | :----: | :--: |
| 0 | WHITE  |  (255, 255, 255) |
| 1 | RED  |  (255, 0, 0) |
| 2 | ORANGE  |  (255, 165, 0) |
| 3 | YELLOW  |  (255, 255, 0) |
| 4 | GREEN  |  (0, 255, 0) |
| 5 | CYAN  |  (0, 255, 255) |
| 6 | BLUE  |  (0, 0, 255)|
| 7 | PURPLE  | (128, 0, 128)|
| 8 | PINK  | (255, 192, 203)|
| 9 | GRAY  | (128, 128, 128)|
| 10 | BLACK  |  (0, 0, 0)|
| 11 | BROWN  |  (165, 42, 42)|
| 12 | OLIVE  |  (128, 128, 0)|
| 13 | TEAL  |(0, 128, 128)|
| 14 | INDIGO  |  (75, 0, 130)|
| 15 | MAGENTA  |  (255, 0, 255)|

## 1. KNOCK(0x20)
Shake hands with HUSKYLENS V2 and tell it whether you're a large-memory MCU or a small-memory MCU.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x20  | 1 byte | 10 |10 bytes |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

**Data**
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1  | boardType<br/>(0 small ram) <br/>(1 large ram) |
| 1-9 | 9 | RFU  |

## 2. GET_RESULT(0x21)
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x20  | 1 byte | 0 |1 byte |

Return：RETURN_INFO RETURN_BLOCK... RETURN_ARROW...

## 3. RFU(0x22)
## 4. RFU(0x23)
## 5. RFU(0x24)
## 6. RFU(0x25)
## 7. RFU(0x26)
## 8. SET_ALGORITHM(0x30)
Switch Algorithm
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x30  | 1 byte | 10 |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | algoID |
| 1 | 1 | 0 |
| 2 | 2 | 0 |
| 4 | 2 | 0 |
| 6 | 2 | 0 |
| 8 | 2 | 0 |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 9. SET_NAME_BY_ID(0x31)
Set the name based on the learned ID (ID > 0). (Assuming the set name length is n, where 0 < n ≤ 32) Supports Chinese characters. For Chinese characters, please use UTF-8 encoding.

| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x31  | 1 byte | 10+1+n |1+n |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | ID（Learnt ID） |
| 1 | 1 | RFU0 |
| 2 | 2 | RFU1 |
| 4 | 2 | RFU2 |
| 6 | 2 | RFU3 |
| 8 | 2 | RFU4 |
| 10 | 1 | name length |
| 11 | n | name |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 10. SET_MULTI_ALGORITHM(0x32)
Create a combination algorithm. Only one combination algorithm may be created. Each combination algorithm supports up to three sub-algorithms.

| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x32  | 0 | 10 |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | multiAlgoNum (Number of multi-algorithms) |
| 1 | 1 | RFU0 |
| 2 | 2 | AlgoID0 |
| 4 | 2 | AlgoID1 |
| 6 | 2 | AlgoID2 |
| 8 | 2 | RFU1 |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 11. SET_MULTI_ALGORITHM_RATIO(0x33)
Set the ratio of algorithm operations in the combination. The number of ratios must match the number of algorithms.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x33  | 0 | 10 |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | multiAlgoNum (Number of multi-algorithms) |
| 1 | 1 | RFU0 |
| 2 | 2 | AlgoID0Ratio |
| 4 | 2 | AlgoID1Ratio |
| 6 | 2 | AlgoID2Ratio |
| 8 | 2 | RFU1 |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 12. SET_ALGO_PARAMS(0x34)
Set algorithm parameters
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x34  | 1 byte | 10+n+m+2 |10 by+n+m+2 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 |  |
| 1 | 1 |  |
| 2 | 2 | arg0(number) |
| 4 | 2 |  |
| 6 | 2 |  |
| 8 | 2 |  |
| 10 | 1 | key（string） length |
| 11 | n |  |
| 11+n | 1 | arg0（string）length |
| 11+n+1 | m | arg0（string） |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 13. RETURN_ARGS(0x45)
Return parameters: up to 4 numeric parameters and 2 string parameters
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data |Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x45  | 1 byte | 10 or 10 + n + m |10 byte|1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | totalIntArgs (Number of int parameters returned) |
| 1 | 1 | Execution result: 0 indicates success, 1 indicates failure. |
| 2 | 2 | arg0_int |
| 4 | 2 |  |
| 6 | 2 |  |
| 8 | 2 |  |
| 10 | 1 | key（string） length |
| 11 | n |  |
| 11+n | 1 | arg0（string）length |
| 11+n+1 | m | arg0（string） |



## 14. RETURN_INFO（0x42）
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x42  | 1 byte | 10 |10 bytes |1 byte |

Data段
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 |  maxID(Maximum number learned)|
| 1 | 1 | RFU0 |
| 2 | 2 | total_results(Total number of identifications) |
| 4 | 2 | total_results_learned(Total number studied) |
| 6 | 2 |  |
| 8 | 2 |  |
| 10 | 1 | key（string） length |
| 11 | n |  |
| 11+n | 1 | arg0（string）length |
| 11+n+1 | m | arg0（string） |

## 15. RETURN_BLOCKS（0x43）
Assume the block's name has length n and its content has length m.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x43  | 1 byte | 10 + 2 + n + m + private_data_length |10 bytes + private_data_length bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | ID |
| 1 | 1 | RFU |
| 2 | 2 | xCenter |
| 4 | 2 | yCenter |
| 6 | 2 | width |
| 8 | 2 | height |
| 10 | 1 | name length |
| 11 | n | name |
| 11+n | 1 | content length |
| 11+n+1 | m | content |
|  |  | Algorithm Private Data |

## 16. RETURN_ARROWS(0x44)
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x44  | 1 byte | 10 |10 bytes |1 byte |

Data
The returned data is a vector. The starting point of the Level 1 vector is the midpoint of the screen's bottom edge (320, 480). The vertical line corresponds to an angle of 0 degrees. Rotating clockwise increases the angle up to 90 degrees, while rotating counterclockwise decreases it down to -90 degrees. The starting point of the Level 2 vector is the endpoint of the Level 1 vector. This design supports operations that add vectors of different levels.
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | ID |
| 1 | 1 | RFU |
| 2 | 2 | xCenter |
| 4 | 2 | yCenter |
| 6 | 2 | angle（-90，90） |
| 7 | 2 | length |



## 17. ACTION_TAKE_PHOTO(0x50)
Take a photo
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x50  | 0 | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.
arg0_str is the photo filename.

## 18. ACTION_TAKE_SCREENSHOT(0x51)
Screenshot
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x51  | 0 | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.
arg0_str is the screenshot filename.

## 19. ACTION_LEARN(0x52)
Learning successful, return ID non-zero; learning failed, return ID 0.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x52  | 0 | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.
arg0_int is the learned ID. 0 indicates not learned, while >=1 indicates the learned ID.

## 20. ACTION_FORGOT(0x53)
Forget everything
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x53  | 0 | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 21. ACTION_SAVE_KNOWLEDGES(0x54)
Save current configuration to the knowledge base
| Head55 | HeadAA  | CMD  | Algo ID | Data Length |Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x54  | 1 byte | 10  |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | knowLedgesIndex |
| 1-9 | 9 | RFU |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 22. ACTION_LOAD_KNOWLEDGES(0x55)
Load Knowledge Base
| Head55 | HeadAA  | CMD  | Algo ID | Data Length |Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x55  | 1 byte | 10  |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | knowLedgesIndex |
| 1 | 9 | RFU |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 23. ACTION_DRAW_RECT(0x56)
Draw a rectangle
| Head55 | HeadAA  | CMD  | Algo ID | Data Length |Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |:--: |
| 0x55 | 0xAA  | 0x56  | 1 byte | 10  |10 bytes |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | colorID |
| 1 | 1 | lineWidth |
| 2 | 2 | x |
| 4 | 2 | y |
| 6 | 2 | w |
| 8 | 2 | h |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 24. ACTION_CLEAN_RECT(0x57)
Clear all rectangles drawn via the communication protocol
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x57  | 1 byte | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 25. ACTION_DRAW_TEXT(0x58)
Clear all text drawn via the communication protocol.
If a background color bgColorID is set, use "bgColorIDFixed = bgColorID + 256".
If no background color is present, use "bgColorIDFixed".
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x58  | 1 byte | 0  |1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | colorID |
| 1 | 1 | lineWidth |
| 2 | 2 | x |
| 4 | 2 | y |
| 6 | 2 | bgColorIDFixed |
| 8 | 2 | RFU0 |
| 10 | 1 | Text length |
| 11 | n | text |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 26. ACTION_CLEAN_TEXT(0x59)
Clear all rectangles drawn via the communication protocol
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x59  | 1 byte | 0  |1 byte |

Return: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 27. ACTION_PLAY_MUSIC(0x5A)
Play music
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Data | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x5A  | 0 | 10+1+n  | 10+1+n bytes | 1byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | RFU0 |
| 1 | 1 | RFU1 |
| 2 | 2 | colume |
| 4 | 2 | RFU2 |
| 6 | 2 | RFU3 |
| 8 | 2 | RFU4 |
| 10 | 1 | name length |
| 11 | n | name |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 28. ACTION_EXIT(0x5B)
Exit the current application. If you are currently on the main menu, you will not exit.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length | Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x5B  | 1 byte | 0  |1 byte |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

## 29. ACTION_LEARN_BLOCK(0x5C)
Set the position and size of the learning box [Currently only available for object tracking]
| Head55 | HeadAA  | CMD  | Algo ID | Data Length |Data| Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x5C  | 1 byte | 10  |10 bytes|1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | RFU0 |
| 1 | 1 | RFU1 |
| 2 | 2 | x1 |
| 4 | 2 | y1 |
| 6 | 2 | x2 |
| 8 | 2 | y2 |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.
arg0_int is the learned ID. 0 indicates not learned, while >=1 indicates the learned ID.

## 30. ACTION_DRAW_UNIQUE_RECT(0x5D)
Drawing a single rectangle will clear any previously drawn rectangles on the screen.
| Head55 | HeadAA  | CMD  | Algo ID | Data Length |Data| Checksum |
| :--: | :----: | :--: | :--: | :--: | :--: |
| 0x55 | 0xAA  | 0x5D  | 1 byte | 10  |10 bytes|1 byte |

Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 1 | colorID |
| 1 | 1 | lineWidth |
| 2 | 2 | x |
| 4 | 2 | y |
| 6 | 2 | w |
| 8 | 2 | h |

Return command: RETURN_ARGS retValue=0 indicates successful execution. Other values indicate execution failure.

# Private data
## FACE_RECOGNITION Private Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 2 | leye_x |
| 2 | 2 | leye_y |
| 4 | 2 | reye_x |
| 6 | 2 | reye_y |
| 8 | 2 | nose_x |
| 10 | 2 | nose_y |
| 12 | 2 | h |
| 14 | 2 | h |
| 16 | 2 | h |
| 18 | 2 | h |

## HAND_RECOGNITION Private Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 2 | wrist_x |
| 2 | 2 | wrist_y |
| 4 | 2 | thumb_cmc_x |
| 6 | 2 | thumb_cmc_y |
| 8 | 2 | thumb_mcp_x |
| 10 | 2 | thumb_mcp_y |
| 12 | 2 | thumb_ip_x |
| 14 | 2 | thumb_ip_y |
| 16 | 2 | thumb_tip_x |
| 18 | 2 | thumb_tip_y |
| 20 | 2 | index_finger_mcp_x |
| 22 | 2 | index_finger_mcp_y |
| 24 | 2 | index_finger_pip_x |
| 26 | 2 | index_finger_pip_y |
| 28 | 2 | index_finger_dip_x |
| 30 | 2 | index_finger_dip_y |
| 32 | 2 | index_finger_tip_x |
| 34 | 2 | index_finger_tip_y |
| 36 | 2 | middle_finger_mcp_x |
| 38 | 2 | middle_finger_mcp_y |
| 40 | 2 | middle_finger_pip_x |
| 42 | 2 | middle_finger_pip_y |
| 44 | 2 | middle_finger_dip_x |
| 46 | 2 | middle_finger_dip_y |
| 48 | 2 | middle_finger_tip_x |
| 50 | 2 | middle_finger_tip_y |
| 52 | 2 | ring_finger_mcp_x |
| 54 | 2 | ring_finger_mcp_y |
| 56 | 2 | ring_finger_pip_x |
| 58 | 2 | ring_finger_pip_y |
| 60 | 2 | ring_finger_dip_x |
| 62 | 2 | ring_finger_dip_y |
| 64 | 2 | ring_finger_tip_x |
| 66 | 2 | ring_finger_tip_y |
| 68 | 2 | pinky_finger_mcp_x |
| 70 | 2 | pinky_finger_mcp_y |
| 72 | 2 | pinky_finger_pip_x |
| 74 | 2 | pinky_finger_pip_y |
| 76 | 2 | pinky_finger_dip_x |
| 78 | 2 | pinky_finger_dip_y |
| 80 | 2 | pinky_finger_tip_x |
| 82 | 2 | pinky_finger_tip_y |

## POSE_RECOGNITION Private Data
| offset | length  | name  |
| :--: | :----: | :--: |
| 0 | 2 | nose_x |
| 2 | 2 | nose_y |
| 4 | 2 | leye_x |
| 6 | 2 | leye_y |
| 8 | 2 | reye_x |
| 10 | 2 | reye_y |
| 12 | 2 | lear_x |
| 14 | 2 | lear_y |
| 16 | 2 | rear_x |
| 18 | 2 | rear_y |
| 20 | 2 | lshoulder_x |
| 22 | 2 | lshoulder_y |
| 24 | 2 | rshoulder_x |
| 26 | 2 | rshoulder_y |
| 28 | 2 | lelbow_x |
| 30 | 2 | lelbow_y |
| 32 | 2 | relbow_x |
| 34 | 2 | relbow_y |
| 36 | 2 | lwrist_x |
| 38 | 2 | lwrist_y |
| 40 | 2 | rwrist_x |
| 42 | 2 | rwrist_y |
| 44 | 2 | lhip_x |
| 46 | 2 | lhip_y |
| 48 | 2 | rhip_x |
| 50 | 2 | rhip_y |
| 52 | 2 | lknee_x |
| 54 | 2 | lknee_y |
| 56 | 2 | rknee_x |
| 58 | 2 | rknee_y |
| 60 | 2 | lankle_x |
| 62 | 2 | lankle_y |
| 64 | 2 | rankle_x |
| 66 | 2 | rankle_y |