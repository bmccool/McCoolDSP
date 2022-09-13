// Uses Declarative syntax to run commands inside a container.
pipeline {
    agent {
        kubernetes {
            containerTemplate {
                name 'shell'
                image 'ubuntu'
                command 'sleep'
                args 'infinity'
            }
            defaultContainer 'shell'
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
