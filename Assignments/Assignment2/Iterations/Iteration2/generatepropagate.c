/*calculate group generates and propagates*/
void group_gen_prop(unsigned int *gi, unsigned int *pi, unsigned int *ggj, unsigned int *gpj){
    unsigned long i=0, j=0, c;
    //printf("here group\n");
    for(i=0; i<MSGSIZE*4/ntasks; i=i+BLKSIZE){  /* Each 4 individual ith bits makes jth group */
        ggj[j] = gi[i+BLKSIZE-1];
        //printf("inside loop\n");
        for(c = BLKSIZE-2; c>=0; c--){
            ggj[j] = ggj[j] | (gi[i+c]&propagateChain(pi, i+BLKSIZE-1, i+c));
            //printf("stop is %d\n",  c);
            if (c==0){ break;}
        }
        //ggj[j] = (gi[i+3]) | (gi[i+2]&pi[i+3]) | (gi[i+1]&pi[i+3]&pi[i+2]) | (gi[i]&pi[i+3]&pi[i+2]&pi[i+1]);
        //gpj[j] = pi[i+3]&pi[i+2]&pi[i+1]&pi[i];
        gpj[j] = propagateChain(pi, i+BLKSIZE-1, i-1);
        //printf("stuck\n");
        j = j+1;
    }

}