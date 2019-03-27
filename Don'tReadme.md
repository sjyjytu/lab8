### Cache Lab

[Here]:(https://ipads.se.sjtu.edu.cn/courses/ics/labs/lab8.pdf) is the requirement detail.



#### Part A

Nothing special to say, it' s easy but troubling.



#### Part B

* 32 x 32: 
  * I devide 32 x 32 into (8 x 4) x (8 x 4), you can refer to my lab8解题思路. Maybe you will be inspired by that.(8 is step, and 4 is part)
* 64 x 64: 
  * First I repeat the way above to solve it (8 x 8) x (8 x 8) , but informed that there are 4600 misses, which meaned it didn' t make sence. 
  * Then I found that each set conflicted with itself, which caused about 4000 misses (I forget the actual number). 
  * Then I try (4 x 16) x (4 x 16) , it came to 1699 misses. Congratulation!!! But there was still some distance from the full point standard (1300). 
  * I though and though again, at last I decided to use a silly way --- store the A data that isn't used with the help of the unused space of B. I got it!
  * It came to 1300 misses, just enough.
* 61 x 67:
  * I wasted so much time to think, finding no special way to fuck it.
  * I was desperate and try the 32 x 32 way but using 4 as step, getting 2300 misses. I knew! This was a silly question.
  * Then I changed the step from 4 to 8 and succeed. What a stupid question!

