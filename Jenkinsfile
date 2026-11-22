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
                echo 'Building...'
                sh 'cd firmware'
                sh 'cmake --preset Debug'
                sh 'cmake --build build/Debug'
            }
        }
        stage('Build Release') {
            steps {
                echo 'Building...'
                sh 'cd firmware'
                sh 'cmake --preset Release'
                sh 'cmake --build build/Release'
            }
        }
        stage('Test') {
            steps {
                echo 'Testing...'
                sh 'cd firmware/test'
                sh 'cmake --build build/test'
                sh 'ctest'
            }
        }
    }
}