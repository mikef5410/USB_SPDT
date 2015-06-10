struct ringBuffer
{
   void** buffer;
   uint64_t writePosition;
   size_t size;
   sem_t* semaphore;
}

//create the ring buffer
struct ringBuffer* buf = calloc(1, sizeof(struct ringBuffer));
buf->buffer = calloc(bufferSize, sizeof(void*));
buf->size = bufferSize;
buf->semaphore = malloc(sizeof(sem_t));
sem_init(buf->semaphore, 0, 0);

//producer
void addToBuffer(void* newValue, struct ringBuffer* buf)
{
   uint64_t writepos = __sync_fetch_and_add(&buf->writePosition, 1) % buf->size;

   //spin lock until buffer space available
   while(!__sync_bool_compare_and_swap(&(buf->buffer[writePosition]), NULL, newValue));
   sem_post(buf->semaphore);
}    

//consumer
void processBuffer(struct ringBuffer* buf)
{
   uint64_t readPos = 0;
   while(1)
   {
       sem_wait(buf->semaphore);

       //process buf->buffer[readPos % buf->size]
       buf->buffer[readPos % buf->size] = NULL;
       readPos++;
   }
}
