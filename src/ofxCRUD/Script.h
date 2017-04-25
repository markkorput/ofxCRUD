#pragma once

namespace ofxCRUD {

    void startScript(const string& path="script/app.py");
    void stopScript();

    class Script : ofThread{

        public:

            static Script* singletonInstance;

        public:
            Script() :  threaded(true),
                        blocking(false),
                        application("python3"),
                        scriptPath("script/app.py"),
                        stdOutPath("script/log.txt"),
                        stdErrPath(""){}

            ~Script(){ stop(); }

            void start();
            void stop();

            void setApplication(const string& app){ application = app; }
            void setScript(const string& script){ scriptPath = script; }
            void setStdOut(const string& path){ stdOutPath = path; }
            void setStdErr(const string& path){ stdErrPath = path; }

        private:
            virtual void threadedFunction();
            string startProcess(bool blocking=false);

        private:
            bool threaded;
            bool blocking;
            string application;
            string scriptPath;
            string stdOutPath;
            string stdErrPath;
    };
}
