// Args.h : descriptions for args

#pragma once

#ifndef ARGS_H_
#define ARGS_H_

class Args {
    private:
        static const int EQUAL_COMPARE = 0;
        static const int ARG_COUNT = 7;
    public:
        static const char DELIMITER = ';';
        static constexpr const char* FLAGS_SHORTENED[ARG_COUNT] = {
            "-h",
            "-r",
            "-p",
            "-x",
            "-e",
            "-d",
            "-nc"
        };
        static constexpr const char* FLAGS_WHOLE[ARG_COUNT] = {
            "--help",
            "--root",
            "--playlist",
            "--exclude",
            "--extensions",
            "--depth",
            "--nocap"
        };
        const enum ArgCodes {
            def = -1,
            help,
            root,
            playlist,
            exclude,
            extensions,
            depth,
            nocap
        };
        /**
        * @brief Checks the provided flag against a list.
        * 
        * @param arg Flag to be checked.
        * @return Code corresponding to the action to be taken.
        */
        static int checkFlag(const char* arg);
};

#endif