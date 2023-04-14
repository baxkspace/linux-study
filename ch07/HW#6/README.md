# feature: slow down coordinate of x/y (key 'q' and 'w')
	If i use variable **delay** of **set_ticker** to implement it, i could not slow down each coordinate of x/y. 

	So I choose to adjust variable **dir**. I set the initial state of dir to 1 in int type, and adjust it to half or double whenever receving q/w/e/r key.

	However, if I use 'q' key or 'w' key in initial state, **ball** moves vertical or horizontal 'cause **ball** receives dir in int data type, so it cannot move only 1/2 coordinate.

	To solve this problem, I choose the way to set limit of the state which 'q' or 'w' key can operate. So in initial state, **ball** cannot slow down only x/y coordinate. To use this feature - slowing down coordinate - we have to speed up each coordinate first.