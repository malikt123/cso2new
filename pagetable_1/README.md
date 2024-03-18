Customizing config.h is quite simple. Go into the file and change the last number on either the "#define LEVELS  ..." or "#define POBITS  ..." lines to change the number of levels or page offset bits, respectively. 

Possible future expansion could include increasing the entry size from 8 bytes or, at least, allowing the user to customize the entry size. Besides that, this code provides sufficient functionality for multi-level translate and page allocate. 

I used test cases and the address sanitizer to test my code and debug. When I was running into issues with segmentation faults, the address sanitizer helped narrow down where my issue was and how I wasn't setting the memory to 0, for example. Additionally, for cases such as when the virtual address is 0 and subsequently translate should return -1, I created test cases to ensure that happened. Lastly, I used lots of print statements while testing to make sure my intended arithmetic was happening. 