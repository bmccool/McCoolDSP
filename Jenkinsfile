// Uses Declarative syntax to run commands inside a container.
pipeline {
    agent {
        kubernetes {
            yaml '''
                apiVersion: v1
                kind: Pod
                metadata:
                spec:
                containers:
                    - name: python
                        image: python:3.10.7-bullseye
                        command:
                            - sleep
                        args:
                            - infinity
                '''
            defaultContainer 'python'
        }
    }
    stages {
        stage('Main') {
            steps {
                sh 'hostname'
                sh 'python --version'
            }
        }
    }
}
