/**
 ******************************************************************************
 * @author: Nark
 * @date:   28/08/2024
 * @brief:  code refactor
 * @copyright: Copyright (c) 2013 Christopher Baker <https://christopherbaker.net>
 ******************************************************************************
**/

#pragma once

#include <stdint.h>
#include <stdio.h>

class CRC32 {
public:
    /* initialize an empty CRC32 checksum */
    CRC32();

    /* reset the checksum claculation */
    void reset();

    /** 
    * @brief: update the current checksum caclulation with the given data
    * @param: <data> - the data to add to the checksum
    **/
    void update(const uint8_t& data);

    /** 
    * @brief: update the current checksum caclulation with the given data
    * @param: <Type> - Type The data type to read
    * @param: <data> - the data to add to the checksum
    **/
    template <typename Type>
    void update(const Type& data) {
        update(&data, 1);
    }

    /** 
    * @brief: update the current checksum caclulation with the given data
    * @param: <Type> - Type The data type to read
    * @param: <data> - the data to add to the checksum
    * @param: <size> - size of the array to add
    **/
    template <typename Type>
    void update(const Type* data, size_t size) {
        size_t nBytes = size * sizeof(Type);
        const uint8_t* pData = (const uint8_t*)data;

        for (size_t i = 0; i < nBytes; i++) {
            update(pData[i]);
        }
    }

    /* the caclulated checksum */
    uint32_t finalize() const;

    /** 
    * @brief: update the current checksum caclulation with the given data
    * @param: <Type> - Type The data type to read
    * @param: <data> - a pointer to the data to add to the checksum
    * @param: <size> - the size of the data to add to the checksum
    * @return: the calculated checksum
    **/
    template <typename Type>
    static uint32_t calculate(const Type* data, size_t size) {
        CRC32 crc;
        crc.update(data, size);
        return crc.finalize();
    }

private:
    /* the internal checksum state */
    uint32_t _state = ~0L;
};