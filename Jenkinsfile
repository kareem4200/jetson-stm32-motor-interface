pipeline {
    agent {label 'stm32-agent'}

    stages {
        stage('Verify Toolchain') {
            steps {
                echo 'Verifying STM32 build environment...'
                sh 'gcc --version'
                sh 'arm-none-eabi-gcc --version'
                sh 'cmake --version'
                sh 'ninja --version'
            }
        }
        stage('Build Debug') {
            steps {
                echo 'Building Debug...'
                dir('firmware') {
                    sh 'cmake --preset Debug'
                    sh 'cmake --build build/Debug'
                }
            }
        }
        stage('Build Release') {
            steps {
                echo 'Building Release...'
                dir('firmware') {
                    sh 'cmake --preset Release'
                    sh 'cmake --build build/Release'
                }
            }
        }
        stage('Test') {
            steps {
                echo 'Running native unit tests...'
                dir('firmware') {
                    sh 'cmake --preset Test'
                    sh 'cmake --build build/Test'
                    sh 'ctest --test-dir build/Test --output-on-failure'
                }
            }
        }
    }
}