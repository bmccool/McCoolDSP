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
                        image: python
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
