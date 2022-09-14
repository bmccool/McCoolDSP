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
        CONAN_PASSWORD = credentials('CONAN_PASSWORD')
    }
    stages {
        stage('Main') {
            steps {
                sh 'hostname'
                sh 'pwd'
                sh 'ls -la'
                sh 'python --version'
                sh 'pip install conan'
                sh 'conan remote add mccool https://brendonmccool.jfrog.io/artifactory/api/conan/mccool-conan-local'
                sh 'conan user -p $CONAN_PASSWORD -r mccool brendonmccool@gmail.com'
                sh 'conan create .'
            }
        }
    }
}
