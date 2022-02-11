import React from "react";
import { Navigate } from "react-router-dom";
import { Container, Card, Form, Row, Col, Button } from 'react-bootstrap';
import { NetworkServices } from "./MessageService";
import { setCookie } from "../Utils/cookie";
import Savetoworkspace from "./Workstation";

class Login extends React.Component {
  constructor(props) {
    super(props);
    this.state = { Name: undefined, redirect: undefined };
  }
  
  handleChange = (event) => {
    this.setState({Name: event.target.value});
  }

  onSubmit = async () => {
    await console.log(Savetoworkspace());
    if(this.state.Name){
      // setCookie("name", this.state.Name, 10);
      this.setState({redirect: '/home/' + this.state.Name});
    }
  }


  render(){
    if (this.state.redirect) {
      return <Navigate to={this.state.redirect}/>;
    }
    return (
      <>
        <Container fluid>
          <Row>
            <h2 className="mx-2 my-2">Welcome to Chat Room!</h2>
          </Row>
          <Row>
            <Card className="p-2 mx-2">
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
