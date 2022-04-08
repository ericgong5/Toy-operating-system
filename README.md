# comp310_A3

CURRENTLY DOING 1.2.2 part 3


PC is the index of the line in the memory
ready_queue_pop()   <- the false means the PCB is not erased from pcb queue, 
true means it is erased and everything is shifted

my_init() <- takes filename and copies content to memory. Also makes PCB and 
puts PCB at end of queue



shellmemory will be our variable store
framestore <- var = filename , value = actual content (char*)


error code 22 = no space in frame store
error code 12 = couldnt open file in backing store   



make sure PC is the same for load_frame_from_disk();
edge case of if PC is out of bounds


#571 <- for backing store testing


promtp for resetmem 
