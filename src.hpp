#ifndef SRC_HPP
#define SRC_HPP

#include <list>

/**
 * Get specified 4096 * n bytes from the memory.
 * @param n
 * @return the address of the block
 */
int* getNewBlock(int n);

/**
 * Free specified 4096 * n bytes from the memory.
 * @param block the pointer to the block
 * @param n
 */
void freeBlock(const int* block, int n);

class Allocator {
private:
    struct Block {
        int* ptr;
        int n_blocks;
        int capacity;
        int used_ints;
        int allocated_ints;
    };

    std::list<Block> blocks;
    Block* current_block;

public:
    Allocator() : current_block(nullptr) {
    }

    ~Allocator() {
        for (auto& b : blocks) {
            freeBlock(b.ptr, b.n_blocks);
        }
        blocks.clear();
        current_block = nullptr;
    }

    /**
     * Allocate a sequence of memory space of n int.
     * @param n
     * @return the pointer to the allocated memory space
     */
    int* allocate(int n) {
        if (n <= 0) return nullptr;

        if (current_block != nullptr && current_block->capacity - current_block->allocated_ints >= n) {
            int* res = current_block->ptr + current_block->allocated_ints;
            current_block->allocated_ints += n;
            current_block->used_ints += n;
            return res;
        }

        long long bytes_needed = (long long)n * sizeof(int);
        int n_blocks = (bytes_needed + 4095) / 4096;
        int* ptr = getNewBlock(n_blocks);
        
        Block b;
        b.ptr = ptr;
        b.n_blocks = n_blocks;
        b.capacity = n_blocks * 4096 / sizeof(int);
        b.used_ints = n;
        b.allocated_ints = n;
        
        blocks.push_back(b);
        current_block = &blocks.back();
        
        return ptr;
    }

    /**
     * Deallocate the memory that is allocated by the allocate member
     * function. If n is not the number that is called when allocating,
     * the behaviour is undefined.
     */
    void deallocate(int* pointer, int n) {
        if (pointer == nullptr || n <= 0) return;

        for (auto it = blocks.begin(); it != blocks.end(); ++it) {
            if (pointer >= it->ptr && pointer < it->ptr + it->capacity) {
                it->used_ints -= n;
                
                if (pointer + n == it->ptr + it->allocated_ints) {
                    it->allocated_ints -= n;
                }
                
                if (it->used_ints == 0) {
                    if (&*it == current_block) {
                        it->allocated_ints = 0;
                    } else {
                        freeBlock(it->ptr, it->n_blocks);
                        blocks.erase(it);
                    }
                }
                return;
            }
        }
    }
};

#endif // SRC_HPP
