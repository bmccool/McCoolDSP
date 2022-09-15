// Uses Declarative syntax to run commands inside a container.
pipeline {
    agent {
        kubernetes {
            yaml '''
apiVersion: v1
kind: Pod
spec:
  containers:
  - name: conan
    image: bmccool/conan:0.1.0
    command:
    - sleep
    args:
    - infinity
'''
            defaultContainer 'conan'
        }
    }
    environment {
        CONAN_PASSWORD = credentials('CONAN_PASS_TAURUS')
    }
    stages {
        stage('Main') {
            steps {
                sh 'hostname'
                sh 'pwd'
                sh 'ls -la'
                sh 'python --version'
                sh 'conan -v'
                sh 'conan remote add mccool https://artifactory-ce.taurus.brendonmccool.com/artifactory/api/conan/conan-local'
                sh 'conan user -p $CONAN_PASSWORD -r mccool admin'
                //sh 'conan profile new default --detect'
                //sh 'conan profile update settings.compiler.libcxx=libstdc++11 default'
                //sh 'conan profile show default'
                sh 'conan create . -e CXXFLAGS="-g -O0"'
            }
        }
    }
}
