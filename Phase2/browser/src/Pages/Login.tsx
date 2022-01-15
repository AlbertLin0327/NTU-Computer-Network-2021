import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Card, Form, Row, Col, Button } from 'react-bootstrap';
import { NetworkServices } from "./MessageService";

interface LoginPageState {
  Name: string | undefined;
  redirect: string | undefined;
}

class Login extends React.Component<{}, LoginPageState> {
  constructor(props: LoginPageState) {
    super(props);
    this.state = { Name: undefined, redirect: undefined };
  }
  
  handleChange = (event: any) => {
    this.setState({Name: event.target.value});
  }

  onSubmit = async () => {
    if(this.state.Name){
      try {
        await NetworkServices.Login(this.state.Name);
      } catch (e) {
        console.log(e);
      }
    }
  }


  render(){
    if (this.state.redirect) {
      return <Navigate to={this.state.redirect} />;
    }
    return (
    <>
      <Container fluid>
        <Row>
          <h2 className="px-2 py-2">Welcome to Chat Room!</h2>
        </Row>
        <Row>
          <Card className="px-2">
            <Card.Title>Please enter your username</Card.Title>
            <Card.Body>
              <Row>
              <Col>
                <Form>
                  <Form.Group controlId="exampleForm.ControlInput1">
                    <Form.Control 
                      type="string" 
                      placeholder="Name"
                      onChange={(e) => this.handleChange(e)}
                    />
                  </Form.Group>
                </Form>
              </Col>
              <Col> 
                <Button variant="outline-primary" onClick={() => this.onSubmit()}>Submit</Button>
              </Col>
              </Row>
            </Card.Body>
          </Card>
        </Row>
      </Container>
    </>
    );
  }
  
}

export default Login;
